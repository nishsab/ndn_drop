#!/usr/bin/python3

import requests
import logging
import random
import os

STATUS = "status"
SUCCESS = "success"
REASON = "reason"
NEIGHBORS = "neighbors"
NEIGHBOR_LIST_PATH = "get_neighbors"
FILES = "files"
FILE_LIST_PATH = "get_file_list"

class FileInfo:
    def __init__(self, filename, modification_time, size, url_path=""):
        self.filename = filename
        self.modification_time = modification_time
        self.size = size
        self.url_path = url_path
    def __repr__(self):
        return str(self.__dict__)

class Backend:
    def __init__(self, url, port):
        self.url = url
        self.port = port
        self.endpoint = "http://{}:{}".format(url, port)
    def neighbor_list_endpoint(self):
        return "{}/{}".format(self.endpoint, NEIGHBOR_LIST_PATH)
    def file_list_endpoint(self, owner):
        return "{}/{}?owner={}".format(self.endpoint, FILE_LIST_PATH, owner)
    def download_file_endpoint(self, url_path):
        return "{}/{}".format(self.endpoint, url_path)
    def get_neighbors_list(self):
        logging.info("Requesting neighbor list")
        endpoint = self.neighbor_list_endpoint()
        j = self.send_request(endpoint)
        logging.info(j)
        return j[NEIGHBORS]
    def get_neighbor_files(self, owner):
        logging.info("Requesting neighbor list from {}".format(owner))
        endpoint = self.file_list_endpoint(owner)
        j = self.send_request(endpoint)
        return j[FILES]
    def download_file(self, url_path):
        logging.info("Downloading file: {}".format(url_path))
        endpoint = self.download_file_endpoint(url_path)
        j = self.send_request(endpoint)
        return j
    def send_request(self, endpoint):
        logging.info("Sending request at {}".format(endpoint))
        j = requests.get(endpoint).json()
        if j.get(STATUS, None) != SUCCESS:
            error_message = j.get(REASON, j.text)
            logging.error("Returned an error ({}): {}".format(j.status_code, error_message))
            raise BackendException(error_message)
        return j

class BackendStub:
    def get_neighbors_list(self):
        return ['laptop', 'phone', 'desktop', 'green', 'home_phone']

class BackendException(Exception):
    pass

class BackendController:
    def __init__(self, backend):
        self.backend = backend
    def get_neighbors(self):
        neighbors = self.backend.get_neighbors_list()
        return neighbors
        filtered_neighbors = [ neighbor for neighbor in neighbors if neighbor != self.home ]
        #deletes = [ index for index in range(len(filtered_neighbors)) if random.randint(0, 4) == 0]
        #while deletes:
        #    filtered_neighbors.pop(deletes.pop())
        return filtered_neighbors
    def get_neighbors_file_list(self, owner):
        file_infos = []
        j = self.backend.get_neighbor_files(owner)

        for file_info_json in j:
            file_infos.append(FileInfo(file_info_json['filename'], int(file_info_json['modification_time']), int(file_info_json['size']), file_info_json['url_path']))
        return file_infos
    def download_file(self, file_info):
        j = self.backend.download_file(file_info.url_path)
        if j["status"] == "success":
            return True, ""
        else:
            return False, j['reason']

class FileSystemController:
    def __init__(self, inbound_directory, outbound_directory):
        self.inbound_directory = inbound_directory
        self.outbound_directory = outbound_directory
    def get_inbound_files(self):
        return self.get_files(self.inbound_directory)
    def get_outbound_files(self):
        return self.get_files(self.outbound_directory)
    def get_files(self, directory_path):
        filenames = os.listdir(directory_path)
        file_infos = []
        for filename in filenames:
            relative_path = "{}/{}".format(directory_path, filename)
            modification_time = int(os.path.getmtime(relative_path))
            size = os.path.getsize(relative_path)
            file_info = FileInfo(filename, modification_time, size)
            file_infos.append(file_info)
        return file_infos




