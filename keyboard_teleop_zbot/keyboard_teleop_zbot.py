#!/usr/bin/env python
import roslib; roslib.load_manifest('keyboard_teleop_zbot')
import rospy

from geometry_msgs.msg import Twist

import sys, select, termios, tty

#Reading from the keyboard  and Publishing to Twist!
#---------------------------
#Moving around:
#   u    i    o
#   j    k    l
#   m    ,    .
#For Holonomic mode (strafing), hold down the shift key:
#---------------------------
#   U    I    O
#   J    K    L
#   M    <    >
#t : up (+z)
#b : down (-z)

msg = """
                 Forward  w

 Left       a                    Right    d

                  Back      s

                  Reset    r
                  
anything else : stop

CTRL-C to quit
"""

moveBindings = {
		'w':(1,0,0,0),
		'r':(1,0,0,-1),
		'd':(0,0,0,-1),
		'a':(0,0,0,1),
		'u':(1,0,0,1),
#		',':(-1,0,0,0),
#		'.':(-1,0,0,1),
		'm':(-1,0,0,-1),
#		'O':(1,-1,0,0),
#		'I':(1,0,0,0),
#		'J':(0,1,0,0),
#		'L':(0,-1,0,0),
#		'U':(1,1,0,0),
		's':(-1,0,0,0),
#		'>':(-1,-1,0,0),
#		'M':(-1,1,0,0),
#		't':(0,0,1,0),
#		'b':(0,0,-1,0),
	       }

#speedBindings={
#		'q':(1.1,1.1),
#		'z':(.9,.9),
#		'w':(1.1,1),
#		'x':(.9,1),
#		'e':(1,1.1),
#		'c':(1,.9),
#	      }

def getKey():
	tty.setraw(sys.stdin.fileno())
	select.select([sys.stdin], [], [], 0)
	key = sys.stdin.read(1)
	termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
	return key

speed = 0.2
turn = 0.3  

def vels(speed,turn):
	return "currently:\tspeed %s\tturn %s " % (speed,turn)

if __name__=="__main__":
    	settings = termios.tcgetattr(sys.stdin)
	
	pub = rospy.Publisher('cmd_vel', Twist, queue_size = 1)
	rospy.init_node('keyboard_teleop_zbot')

	x = 0
	y = 0
	z = 0
	th = 0
	status = 0

	try:
		print msg
		print vels(speed,turn)
		while(1):
			key = getKey()
			if key in moveBindings.keys():
				x = moveBindings[key][0]
				y = moveBindings[key][1]
				z = moveBindings[key][2]
				th = moveBindings[key][3]
#			elif key in speedBindings.keys():
#				speed = speed * speedBindings[key][0]
#				turn = turn * speedBindings[key][1]#

#				print vels(speed,turn)
#				if (status == 14):
#					print msg
#				status = (status + 1) % 15
			else:
				x = 0
				y = 0
				z = 0
				th = 0
				if (key == '\x03'):
					break
			twist = Twist()					
			if(y==1):
				twist.linear.x=x;
				twist.linear.y=y;
				twist.linear.z=z;
				twist.angular.z=th;
			else:				
				twist.linear.x = x*speed; twist.linear.y = y*speed; twist.linear.z = z*speed;
				twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = th*turn
			pub.publish(twist)

	except:
		print e

	finally:
		twist = Twist()
		twist.linear.x = 0; twist.linear.y = 0; twist.linear.z = 0
		twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = 0
		pub.publish(twist)

    		termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)


