#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
import sys
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from PyQt4.QtCore import pyqtSlot,SIGNAL,SLOT
import random
 
class QProgBar(QProgressBar):
 
    value = 0
 
    @pyqtSlot()
    def increaseValue(progressBar):
        progressBar.setValue(random.randrange(0,100))
        progressBar.value = progressBar.value+1
 
# Create an PyQT4 application object.
a = QApplication(sys.argv)       
 
# The QWidget widget is the base class of all user interface objects in PyQt4.
w = QWidget()
 
# Set window size. 
w.resize(320, 240)
 
# Set window title  
w.setWindowTitle("PyQT4 Progressbar @ pythonspot.com ") 
 
# Create progressBar. 
bar = QProgBar(w)
bar.resize(320,50)    
bar.setValue(0)
bar.move(0,10)


# create timer for progressBar
timer = QTimer()
bar.connect(timer,SIGNAL("timeout()"),bar,SLOT("increaseValue()"))
timer.start(100) 
 
# Show window
w.show() 
 
sys.exit(a.exec_())
