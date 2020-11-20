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

BLOCK_SIZE=8000
FRESHNESS_PERIOD = 0#always stale

class FileInfo:
    def __init__(self, filename, relative_path):
        self.filename = filename
        self.relative_path = relative_path
        self.modification_time = 0
        self.size = 0
        self.num_segs = 0
    def __repr__(self):
        return str(self.__dict__)
        
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

tracked_files = {}

if len(sys.argv) != 3:
    print("Usage: python3 FileManager.py DIR_PATH PREFIX")
    exit()

dir_path = sys.argv[1]
prefix = sys.argv[2]

class DirectoryMonitor:
    def __init__(self, dir_path):
        self.tracked_files = {}
        self.dir_path = dir_path
    def check_for_new_files():
        logging.info("Checking {} for new files".format(self.dir_path))
        filenames = os.listdir(self.dir_path)
        new_files = []
        for filename in filenames:
            relative_path = "%s/%s" % (self.dir_path, filename)
            modification_time = int(os.path.getmtime(relative_path))
            if not filename in self.tracked_files:
                self.tracked_files[filename] = FileInfo(filename, relative_path)
            file_info = self.tracked_files[filename]
            if modification_time > file_info.modification_time:
                file_info.modification_time = modification_time
                file_info.size = os.path.getsize(relative_path)
                file_info.num_segs = math.ceil(file_info.size / BLOCK_SIZE)
                logging.info("Found a new file: {}".format(file_info))
                if file_info.size > 0:
                    send_file_to_repo(file_info)
                    new_files.append(deepcopy(file_info))
        return new_files

class EncodedPacket:
    def __init__(self, name, data, freshness_period, final_block_id, app):
        self.name = name
        self.data = data
        self.freshness_period = freshness_period
        self.final_block_id = final_block_id
        self.encoded_packet = app.prepare_data(name, data, freshness_period=freshness_period, final_block_id=final_block_id)
    
class PacketEncoder:
    def __init__(self, app, freshness_period, block_size):
        self.app = app
        self.freshness_period
        self.block_size = block_size
    def encode_packets(self, file_infos):
        encoded_packets = []
        for file_info in file_infos:
            file_packets = self.encoded_file_into_packets(file_info)
            encoded_packets.extend(file_packets)
        return encoded_packets
    def encode_file_into_packets(self, file_info):
        logging.info("Encoding {} into {} packets".format(file_info.filename, file_info.num_segs))
        encoded_packets = []
        with open(file_info.relative_path, 'rb') as infile:
            name = Name.normalize(file_info.filename)
            name.append(Component.from_version(file_info.modification_time))
            logging.info("Segmenting file under {}".format(Name.to_str(name)))
            
            data = infile.read()
            final_block_id = Component.from_segment(file_info.num_segs-1)
            
            for index in range(file_info.num_segs):
                encoded_packet = EncodedPacket(name + [Component.from_segment(i)],
                                                data[i*segment_size:(i+1)*self.block_size],
                                                self.freshness_period,
                                                final_block_id,
                                                self.app)
                logging.info("Encoded packet: {}".format(Name.to_str(encoed_packet.name)))
                encoded_packets.append(encoded_packet)
        return encoded_packets

class PacketSender:
    def __init__(self, hostname, port):
        self.hostname = hostname
        self.port = port
    def send_packets(self, encoded_packets):
        for encoded_packet in encoded_packets:
            netcat(self.hostname, self.port, encoded_packet.encoded_packet)
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
    
def monitor_directory():
    logging.info("Checking {} for new files".format(dir_path))
    filenames = os.listdir(dir_path)
    for filename in filenames:
        relative_path = "%s/%s" % (dir_path, filename)
        modification_time = int(os.path.getmtime(relative_path))
        if not filename in tracked_files:
            tracked_files[filename] = FileInfo(filename, relative_path)
        file_info = tracked_files[filename]
        if modification_time > file_info.modification_time:
            file_info.modification_time = modification_time
            file_info.size = os.path.getsize(relative_path)
            file_info.num_segs = math.ceil(file_info.size / BLOCK_SIZE)
            logging.info("Found a new file: {}".format(file_info))
            if file_info.size > 0:
                send_file_to_repo(file_info)

app = NDNApp()
def _create_packet(name, data, freshness_period, final_block_id):
    packet = app.prepare_data(name, data, freshness_period=freshness_period, final_block_id=final_block_id)
    return packet

def send_file_to_repo(file_info):
    logging.info("Sending {} to repo".format(file_info.filename))
    with open(file_info.relative_path, 'rb') as infile:
        name = Name.normalize(file_info.filename)
        name.append(Component.from_version(file_info.modification_time))
        logging.info("Segmenting file under {}".format(Name.to_str(name)))

        data = infile.read()
        freshness_period = FRESHNESS_PERIOD
        segment_size = BLOCK_SIZE
        final_block_id = Component.from_segment(file_info.num_segs-1)

        packets = [[name + [Component.from_segment(i)], data[i*segment_size:(i+1)*segment_size]]
            for i in range(file_info.num_segs)]

        for packet_name, packet_data in packets:
            logging.info("Encoding packet: {}".format(Name.to_str(packet_name)))
            encoded_packet = _create_packet(packet_name, packet_data, freshness_period, final_block_id)
            logging.info("Sending packet: {}".format(Name.to_str(packet_name)))
            netcat("localhost", 7376, encoded_packet)
            
        with multiprocessing.Pool(processes=multiprocessing.cpu_count()) as p:
            res = p.starmap(_create_packets, packets)
            packet_list = res
        num_packets = len(packet_list)
        print('Created {} chunks under name {}'.format(num_packets, file_info.filename))
        for encoded_packet in packet_list:
            print(encoded_packet)
        #    outfile.write(encoded_packet)
        #print("written {} packets to file {}".format(num_packets, args.output_file))
        #print("You can use  \"nc localhost 7376 < {}\" to load data into the repo".format(args.output_file))    

if __name__ == '__main__':
    logging.basicConfig(format='{asctime} {levelname}: {message}',
                        datefmt='%Y-%m-%d %H:%M:%S',
                        level=logging.INFO,
                        style='{')
    logging.info("Starting up")
    while True:
        monitor_directory()
        time.sleep(1)

import logging
import sys
from ndn.utils import timestamp
from ndn.app import NDNApp
from ndn.encoding import Name, Component
import multiprocessing
import argparse

app = NDNApp()
def _create_packets(name, data, freshness_period, final_block_id):
    packet = app.prepare_data(name, data, freshness_period=freshness_period, final_block_id=final_block_id)
    return packet

def main():
    """Convert a file into tlv packets. Store them in the output file one 
    after the other that can be dumped into an NDN repo using netcat"""
    
    cpu_count = multiprocessing.cpu_count()
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--name', type=str, required=True)
    parser.add_argument('-i', '--input_file', type=str, required=True)
    parser.add_argument('-o', '--output_file', type=str, required=True)
    parser.add_argument('-s', '--segment_size', type=int, default=8000, required=False)
    parser.add_argument('-f', '--freshness_period', type=int, default=0, required=False) #always stale
    parser.add_argument('-c', '--cpu_count', type=int, default=cpu_count, required=False)

    args = parser.parse_args()

    logging.basicConfig(format='[{asctime}]{levelname}:{message}',
                        datefmt='%Y-%m-%d %H:%M:%S',
                        level=logging.INFO,
                        style='{')

    segment_size = args.segment_size
    name = Name.normalize(args.name)
    name.append(Component.from_version(timestamp()))

    print("Converting {} into tlv packets and storing in {} \nSegment size = {}, Freshness Period = {}, CPU_Count = {}"
            .format(args.input_file, args.output_file, args.segment_size, args.freshness_period, args.cpu_count))


    with open(args.input_file, 'rb') as infile, open(args.output_file, 'wb') as outfile:
        data = infile.read()
        seg_cnt = (len(data) + segment_size - 1) // segment_size
        freshness_period = args.freshness_period
        final_block_id = Component.from_segment(seg_cnt-1)
        packets = [[name + [Component.from_segment(i)], data[i*segment_size:(i+1)*segment_size], 
            freshness_period, final_block_id] 
            for i in range(seg_cnt)]
        with multiprocessing.Pool(processes=args.cpu_count) as p:
            res = p.starmap(_create_packets, packets)
            packet_list = res
        num_packets = len(packet_list)
        print('Created {} chunks under name {}'.format(num_packets, args.name))
        for encoded_packet in packet_list:
            outfile.write(encoded_packet)
        print("written {} packets to file {}".format(num_packets, args.output_file))
        print("You can use  \"nc localhost 7376 < {}\" to load data into the repo".format(args.output_file))
