import time

class AUTObot:
	def __init__(self, pos, mass, path, max_speed, acceleration, friction):
		Disc.Disc.__init__(self, pos, 0, 0, mass, path, max_speed, friction)
		self.__acceleration = acceleration
		self.__point = point
	def get_acceleration(self):
		return self.__acceleration
	def get_point(self):
		return self.__point
	def set_acceleration(self, acceleration):
		self.__acceleration = acceleration
	def set_point(self, point):
		self.__point = point

	def forward(duration):
		y = 0
		for i in range(0,duration):
			time.sleep(.001)
			y-=1
		return y

	def backward(duration):
		y = 0
		for i in range(0,duration):
			time.sleep(.001)
			y+=1
		return x
	def right(duration):
		x = 0
		for i in range(0,duration):
			time.sleep(.001)
			x+=1
		return x

	def left(duration):
		x = 0
		for i in range(0,duration):
			time.sleep(.001)
			x-=1
		return x

	#def rotateCW(duration):
		
	#def rotateCCW(duration):