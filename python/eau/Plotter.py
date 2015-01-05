import matplotlib.pyplot as plt
import numpy as np

class Plotter:
    def __init__(self):
        self.__fig = plt.figure().number
        return

    def scatterPlot( self, data, alpha=0.01 ):
        self.scatterPlotXY( data[0:,0], data[0:,1], alpha )
        return

    def scatterPlotXY( self, x, y, alpha=0.01 ):
        plt.figure( self.__fig )
        plt.clf()
        plt.plot(x,y, 'ro', alpha=alpha )
        plt.draw()
        plt.show(block=False)
        return

    def timeSeries( self, data, asline = True ):
        x = np.arange(0,len(data))
        plt.figure( self.__fig )
        plt.clf()
        if asline:
            plt.plot(x,data, 'g--' )
        else:
            plt.bar(x,data)
        plt.draw()
        plt.show(block=False)
        return

    def histogram( self, data, bins=50, same=False ):
        plt.figure( self.__fig )
        if not same:
            plt.clf()
        plt.hist(data, bins, normed=1, alpha=0.80 )            
        plt.draw()
        plt.show(block=False)
        return
                

    def __del__(self):
        plt.close( self.__fig )
        return
