#!/usr/bin/python3

from typing import Optional
from ndn.app import NDNApp
from ndn.encoding import Name, InterestParam, BinaryStr, FormalName, MetaInfo, Component
import multiprocessing
import logging
import sys
import os
from pathlib import Path
import math
import time
import sys, socket
from copy import deepcopy
import argparse 
import json

class FileInfo:
    def __init__(self, filename, relative_path, prefix, block_size):
        self.filename = filename
        self.relative_path = relative_path
        self.modification_time = 0
        self.size = 0
        self.num_segs = 0
        self.prefix = prefix
        self.block_size = block_size
    def get_ndn_name(self):
        return "{}/{}/v={}".format(self.prefix, self.filename, self.modification_time)
    def __repr__(self):
        return str(self.__dict__)
        
class FileMetadata:
    def __init__(self, file_info):
        self.filename = file_info.filename
        self.ndn_name = file_info.get_ndn_name()
        self.modification_time = file_info.modification_time
        self.size = file_info.size
        self.num_segs = file_info.num_segs
    def toJSON(self):
        return json.dumps(self, default=lambda o: o.__dict__, 
            sort_keys=True, indent=4)

class DirectoryMonitor:
    def __init__(self, dir_path, block_size, prefix, metadata_path):
        self.tracked_files = {}
        self.dir_path = dir_path
        self.block_size = block_size
        self.prefix = prefix
        self.metadata_path = metadata_path
        self.metadata_tmp = self.metadata_path + ".tmp"
    def check_for_new_files(self):
        logging.info("Checking {} for new files".format(self.dir_path))
        filenames = os.listdir(self.dir_path)
        new_files = []
        for filename in filenames:
            relative_path = "%s/%s" % (self.dir_path, filename)
            modification_time = int(os.path.getmtime(relative_path))
            if not filename in self.tracked_files:
                self.tracked_files[filename] = FileInfo(filename, relative_path, self.prefix, self.block_size)
            file_info = self.tracked_files[filename]
            if modification_time > file_info.modification_time:
                file_info.modification_time = modification_time
                file_info.size = os.path.getsize(relative_path)
                file_info.num_segs = math.ceil(file_info.size / self.block_size)
                logging.info("Found a new file: {}".format(file_info))
                if file_info.size > 0:
                    new_files.append(deepcopy(file_info))
        return new_files
    def persist(self):
        file_infos = list(self.tracked_files.values())
        data = json.dumps({"files": file_infos}, default = lambda x: x.__dict__, indent=4, sort_keys=True)
        with open(self.metadata_tmp, "w") as fd:
            fd.write(data)
        os.rename(self.metadata_tmp, self.metadata_path)
        


class EncodedPacket:
    def __init__(self, original_name, name, data, freshness_period, final_block_id, app):
        self.original_name = original_name
        self.name = name
        self.data = data
        self.freshness_period = freshness_period
        self.final_block_id = final_block_id
        self.encoded_packet = app.prepare_data(name, data, freshness_period=freshness_period, final_block_id=final_block_id)
    
class PacketEncoder:
    def __init__(self, app, prefix, freshness_period):
        self.app = app
        self.prefix = prefix
        self.freshness_period = freshness_period
    def encode_packets(self, file_infos):
        encoded_packets = []
        for file_info in file_infos:
            file_packets = self.encode_file_into_packets(file_info)
            encoded_packets.extend(file_packets)
        return encoded_packets
    def encode_file_into_packets(self, file_info):
        num_segs = file_info.num_segs
        logging.info("Encoding {} into {} packets".format(file_info.filename, file_info.num_segs))
        encoded_packets = []
        with open(file_info.relative_path, 'rb') as infile:
            name_components = [ comp for comp in file_info.get_ndn_name().split("/") if comp ]
            name = Name.normalize(name_components)
            
            logging.info("Segmenting file under {}".format(Name.to_str(name)))
            
            data = infile.read()
            final_block_id = Component.from_segment(num_segs-1)
            
            for i in range(num_segs):
                full_name = name + [Component.from_segment(i)]
                encoded_packet = EncodedPacket(Name.to_str(full_name), full_name,
                                                data[i*file_info.block_size:(i+1)*file_info.block_size],
                                                self.freshness_period,
                                                final_block_id,
                                                self.app)
                logging.info("Encoded packet: {}".format(encoded_packet.original_name))
                encoded_packets.append(encoded_packet)
        return encoded_packets

class PacketSender:
    def __init__(self, hostname, port):
        self.hostname = hostname
        self.port = port
    def send_packets(self, encoded_packets):
        for encoded_packet in encoded_packets:
            self.netcat(self.hostname, self.port, encoded_packet.encoded_packet)
    @staticmethod
    def netcat(host, port, content):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((host, int(port)))
        s.sendall(content)
        s.shutdown(socket.SHUT_WR)
        while True:
            data = s.recv(4096)
            if not data:
                break
            print(repr(data))
        s.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Monitor a directory and send any new/updated files to an ndn repo")
    parser.add_argument('-d', '--dir_path', type=str, default="outbound_dir", required=False, help="path to directory to monitor")
    parser.add_argument('-m', '--metadata_file', type=str, default="outbound_dir", required=False, help="path to metadata file to write to")
    parser.add_argument('-p', '--prefix', type=str, default="", required=False, help="prefix for creating ndn name")
    parser.add_argument('-f', '--freshness_period', type=int, default=0, required=False) #always stale
    parser.add_argument('-s', '--segment_size', type=int, default=8000, required=False)
    parser.add_argument('-i', '--host', type=str, default="localhost", required=False, help="ip address for tcp bulk insert of repo")
    parser.add_argument('-o', '--port', type=int, default=7376, required=False, help="port for tcp bulk insert of repo")

    args = parser.parse_args()

    logging.basicConfig(format='{asctime} {levelname}: {message}',
                        datefmt='%Y-%m-%d %H:%M:%S',
                        level=logging.INFO,
                        style='{')
    logging.info("Starting up")
    
    directory_monitor = DirectoryMonitor(args.dir_path, args.segment_size, args.prefix, args.metadata_file)
    app = NDNApp()
    packet_encoder = PacketEncoder(app, args.prefix, args.freshness_period)
    packet_sender = PacketSender(args.host, args.port)
    while True:
        new_files = directory_monitor.check_for_new_files()
        encoded_packets = packet_encoder.encode_packets(new_files)
        packet_sender.send_packets(encoded_packets)
        directory_monitor.persist()
        time.sleep(1)
