#!/usr/bin/python3
import argparse

from BackendLayer import Backend, BackendStub, BackendController, FileSystemController
import logging
from MultiScreenApp import MultiScreenApp


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a frontend GUI for a backend NDNDrop instance listening on json endpoints")
    parser.add_argument('-o', '--outbound_dir', type=str, required=True, help="path to outbound directory")
    parser.add_argument('-i', '--inbound_dir', type=str, required=True, help="path to inbound directory")
    parser.add_argument('-s', '--hostname', type=str, required=True, help="ip address for ndndrop instance")
    parser.add_argument('-p', '--port', type=int, required=True, help="port for ndndrop instance")
    parser.add_argument('-n', '--name', type=str, default="Local Node", required=False, help="name of ndn drop node")

    args = parser.parse_args()

    logging.basicConfig(format='{asctime} {levelname}: {message}',
                        datefmt='%Y-%m-%d %H:%M:%S',
                        level=logging.INFO,
                        style='{')
    logging.info("Starting up")

    backend = Backend(args.hostname, args.port)
    #backend = BackendStub()
    neighbor_list = backend.get_neighbors_list()
    backend_controller = BackendController(backend)
    file_system_controller = FileSystemController(args.inbound_dir, args.outbound_dir)

    app = MultiScreenApp(backend_controller, file_system_controller, args.name)
    app.mainloop()
