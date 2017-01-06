import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import sys, string, os
from time import gmtime, strftime

import tkinter as tk
from tkinter import filedialog

import json
from pprint import pprint

# File location of this script
__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__))) + "/"


# First plot output from Exercise 1
with open(__location__ + '/results') as resultsFile:
    lines = resultsFile.readlines()

# Strip off the new lines
lines = [line.rstrip('\n') for line in lines]



# First line holds the x values
# Second line holds the y values
# First identifier in each line holds the name of the axis
xline = lines[0].split(',')
yline = lines[1].split(',')


xvalues = [float(i) for i in xline[1:]]
yvalues = [float(i) for i in yline[1:]]

xlabel = xline[0]
ylabel = yline[0]

# print('X: ' + xlabel + ' - ' + str(xvalues))
# print('Y: ' + ylabel + ' - ' + str(yvalues))

# Plot the values into a pdf file and open it

# PDF
pdfPath = __location__ + 'results.pdf'

# Perf
pp = PdfPages(pdfPath)

plt.plot(xvalues, yvalues)
plt.xlabel(xlabel)
plt.ylabel(ylabel)
plt.grid(True)

pp.savefig()
plt.savefig('results.png');
pp.close()

if sys.platform == 'linux2':
    subprocess.call(["xdg-open", pdfPath])
else:
    os.startfile(pdfPath)