import tkinter as tk
from tkinter import messagebox


class Gui:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("ModLab")
        self.root.geometry("800x400")
        self.root.resizable(0,0)
        self.root.update()
    def mklabel(self, name, text, col = 0, row = 0, pady=3, padx=3):
        label = tk.Label(self.root, text = text, font = ("Arial", 12))
        label.grid(row = row, column = col, pady = pady, padx = padx)
    def mkbutton(self, name, text, col = 0, row = 0, command = None, pady = 3, padx = 3):
        if command is None:
            button = tk.Button(self.root, text = text, font = ("Arial", 12))
        else:
            button = tk.Button(self.root, text = text, font = ("Arial", 12), command = command)
        button.grid(row = row, column = col, pady = pady, padx = padx)
    def popup(self):
        self.popup = tk.Toplevel(self.root)
        self.popup.geometry("400x200")
        self.popup.title("Popup")
        self.popup.resizable(0,0)



demo = {
    "a": "b",
    "b": "c",
    "c": "d",
    "d": "e",
    "e": "f",
    "f": "g",
}
def message():
    messagebox.showinfo("a", "b")


if __name__ == "__main__":
    gui1 = Gui(); gui1.mkbutton("Demo button", "Show message", command=message)
    [gui1.mklabel(a, f"Demo: {a} | {b}", i, i) for i, (a, b) in enumerate(demo.items())]
    gui1.root.mainloop()
