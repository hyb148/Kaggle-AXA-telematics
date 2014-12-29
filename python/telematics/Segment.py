# Segment class
class Segment:
    def __init__(self, coordinates ):
        self.__data = coordinates
        return

    def rawData(self):
        return self.__data

    def size(self):
        return len(self.__data)
    
