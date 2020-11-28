#!/usr/bin/python

import tkinter as tk
import datetime
import logging

REMOTE_FILES_NAME = "remote_files_name"
LARGEFONT = ("TkDefaultFont", 25)
MEDIUMFONT = ("TkDefaultFont", 20)


class MultiScreenApp(tk.Tk):
    def __init__(self, backend_controller, filesystem_controller, node_name, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        self.title("NDN Drop GUI for {}".format(node_name))

        self.container = tk.Frame(self)
        self.backend_controller = backend_controller
        self.filesystem_controller = filesystem_controller

        self.container.pack(side="top", fill="both", expand = True)
        self.wm_geometry("800x800")

        self.container.grid_rowconfigure(0, weight=1)
        self.container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (StartPage, NeighborsPage):
            frame = F(self.container, self)
            self.frames[F] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        self.show_frame(StartPage)
    def show_frame(self, cont):
        if "key" in self.frames:
            self.frames["key"].destroy()
            self.frames.pop("key")
        if REMOTE_FILES_NAME in self.frames:
            self.frames[REMOTE_FILES_NAME].destroy()
            self.frames.pop(REMOTE_FILES_NAME)
        frame = self.frames[cont]
        frame.tkraise()
    def push_remote_files_page(self, neighbor_name):
        frame = RemoteFilesPage(self.container, self, neighbor_name)
        self.frames[REMOTE_FILES_NAME] = frame
        frame.grid(row=0, column=0, sticky="nsew")
    def push_new_frame(self, frame_class):
        frame = frame_class(self.container, self)
        self.frames["key"] = frame
        frame.grid(row=0, column=0, sticky="nsew")

class FileDisplay:
    def __init__(self, file_info):
        self.file_info = file_info
        self.display_length = 0
        timestamp = datetime.datetime.fromtimestamp(self.file_info.modification_time).strftime('%Y-%m-%d %H:%M:%S')
        self.display = "{:<20} {}    {}B".format(self.file_info.filename, timestamp, self.file_info.size)
    def __repr__(self):
        return self.display
    def __str__(self):
        return self.__repr__()

class RemoteFilesPage(tk.Frame):
    def __init__(self, parent, controller, neighbor_name):
        tk.Frame.__init__(self, parent)
        self.backend_controller = controller.backend_controller
        self.neighbor_name = neighbor_name
        self.file_displays = {}

        buttonframe = tk.Frame(self)
        buttonframe.pack(side="top", fill="x", expand=False)
        start_page_button = tk.Button(buttonframe, text="View Files", command=lambda: controller.show_frame(StartPage))
        neighbors_page_button = tk.Button(buttonframe, text="View Neighbors", command=lambda: controller.show_frame(NeighborsPage))
        start_page_button.pack(side="left")
        neighbors_page_button.pack(side="left")

        label = tk.Label(self, text="{}'s Files".format(neighbor_name), font=LARGEFONT)
        label.pack()

        self.listbox = tk.Listbox(self, font="Courier")
        self.listbox.pack(side="top", fill="both", expand=True, padx=10)

        def download_file():
            display_name = self.listbox.get(self.listbox.curselection())
            logging.info("Downloading {}".format(display_name))
            display = self.file_displays[display_name]
            status, reason = self.backend_controller.download_file(display.file_info)

            if status:
                logging.info("{} was succesfully downloaded.".format(display.file_info.filename))
                success_label = tk.Label(self, text="Success!", foreground="green")
            else:
                logging.error("{} download failed! Reason: {}".format(display.file_info.filename, reason))
                success_label = tk.Label(self, text="Failed: ".format(reason), foreground="red")
            success_label.pack()
            self.after(1000, lambda: success_label.destroy())

        btn = tk.Button(self, text='Download File', command=download_file)
        btn.pack()

        self.poll()
    def poll(self):
        file_infos = self.backend_controller.get_neighbors_file_list(self.neighbor_name)
        file_infos.sort(key=lambda x: x.filename)
        file_displays = [ FileDisplay(file_info) for file_info in file_infos ]
        for index, file_display in enumerate(file_displays):
            while index < self.listbox.size() and self.listbox.get(index) < file_display.display:
                self.listbox.delete(index)
            if self.listbox.get(index) != file_display.display:
                self.file_displays[file_display.display] = file_display
                self.listbox.insert(index,  file_display)
        self.listbox.delete(len(file_infos))
        self.after(5000, self.poll)

class NeighborsPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.backend_controller = controller.backend_controller

        buttonframe = tk.Frame(self)
        buttonframe.pack(side="top", fill="x", expand=False)
        start_page_button = tk.Button(buttonframe, text="View Files", command=lambda: controller.show_frame(StartPage))
        neighbors_page_button = tk.Button(buttonframe, text="View Neighbors", command=lambda: controller.show_frame(NeighborsPage))
        start_page_button.pack(side="left")
        neighbors_page_button.pack(side="left")

        label = tk.Label(self, text="Online Neighbors", font=LARGEFONT)
        label.pack()

        self.listbox = tk.Listbox(self, font="Courier")
        self.listbox.pack()

        def dialog():
            neighbor_name = self.listbox.get(self.listbox.curselection())
            controller.push_remote_files_page(neighbor_name)

        btn = tk.Button(self, text='View Files', command=dialog)
        btn.pack()

        self.poll()

    def poll(self):
        neighbors = self.backend_controller.get_neighbors()
        neighbors.sort()
        for index, neighbor in enumerate(neighbors):
            while index < self.listbox.size() and self.listbox.get(index) < neighbor:
                self.listbox.delete(index)
            if self.listbox.get(index) != neighbor:
                self.listbox.insert(index, neighbor)
        self.listbox.delete(len(neighbors))
        self.after(3000, self.poll)

class StartPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.filesystem_controller = controller.filesystem_controller

        buttonframe = tk.Frame(self)
        buttonframe.pack(side="top", fill="x", expand=False)
        start_page_button = tk.Button(buttonframe, text="View Files", command=lambda: controller.show_frame(StartPage))
        neighbors_page_button = tk.Button(buttonframe, text="View Neighbors", command=lambda: controller.show_frame(NeighborsPage))
        start_page_button.pack(side="left")
        neighbors_page_button.pack(side="left")

        label = tk.Label(self, text="Local Files", font=LARGEFONT)
        label.pack(pady=4, padx=10)

        filebox_frame = tk.Frame(self)
        filebox_frame.pack(side="top", fill="both", expand=True, padx=10)
        left_frame = tk.Frame(filebox_frame)
        right_frame = tk.Frame(filebox_frame)
        left_frame.pack(side="left", fill="both", expand=True)
        right_frame.pack(side="left", fill="both", expand=True)

        left_label = tk.Label(left_frame, text="Inbound Directory", font=MEDIUMFONT)
        left_label.pack(side="top")
        self.inbound_listbox = tk.Listbox(left_frame, font="Courier")
        self.inbound_listbox.pack(fill="both", expand=True, pady=3, padx=3)

        right_label = tk.Label(right_frame, text="Outbound Directory", font=MEDIUMFONT)
        right_label.pack(side="top")
        self.outbound_listbox = tk.Listbox(right_frame, font="Courier")
        self.outbound_listbox.pack(fill="both", expand=True, pady=3, padx=3)

        self.poll()
    def poll(self):
        self.update_listbox(self.inbound_listbox, self.filesystem_controller.get_inbound_files)
        self.update_listbox(self.outbound_listbox, self.filesystem_controller.get_outbound_files)
        self.after(3000, self.poll)
    @staticmethod
    def update_listbox(listbox, file_getter):
        file_infos = file_getter()
        file_infos.sort(key=lambda x: x.filename)
        file_displays = [ FileDisplay(file_info) for file_info in file_infos ]
        for index, file_display in enumerate(file_displays):
            while index < listbox.size() and listbox.get(index) < file_display.display:
                listbox.delete(index)
            if listbox.get(index) != file_display.display:
                listbox.insert(index,  file_display)
        listbox.delete(len(file_infos))