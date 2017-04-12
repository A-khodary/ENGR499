import pygame
import sys
import os
import pid_controller
import AutoBot

#import xlsxwriter

import math

from pygame.locals import *
from Chrono import Chrono
from Mallet import Mallet
from Goalpost import Goalpost
from Disc import Disc
from const import *

from random import randint

import numpy as np
import matplotlib.pyplot as plt


def init():
    angle = 0
    player1.set_pos_xy(PLAYER1_START)
    player1.set_speed_magnitude(0)
    if angle == 0:
        player1.set_point(0)
        chrono.reset()
    goal = (goal_x, goal_y)


    

def init_graph():
    fig1 = plt.figure()

    data = np.random.rand(2, 25)
    for i in range(0,len(xloc)):
        xloc[i]-=400
        yloc[i]*=-1
        yloc[i]+=360

    plt.plot(xloc, yloc, 'r-')
    plt.xlim(-400, 400)
    plt.ylim(-400, 400)
    plt.xlabel('x')
    plt.title('Robot Location')
    plt.show()  

    fig2 = plt.figure()
    plt.plot(plot_time, plot_error)
    plt.xlim(0, 25000)
    #plt.ylim(-40, 40)
    plt.title('Controller Output v. Time')
    plt.show()

def debug():
    for i in range(0,len(xloc)):
        print("x = ", xloc[i], " y = ", yloc[i], file=sys.stderr) 



def game():

    f = open('controller output.txt', 'w')

    #workbook = xlsxwriter.Workbook('Controller Output.xlsx')
    #worksheet = workbook.add_worksheet()

    row = 2
    col = 0


    init()

    while 1:
        screen.blit( bg, (0,0))

        for event in pygame.event.get():
            if event.type == QUIT:
                sys.exit()

        goal_theta = math.atan2(goal_y - player1.getY(), player1.getX()-goal_x)
        goal_theta = math.degrees(goal_theta)

        distance = math.sqrt(math.pow(goal_x - player1.getX(), 2) + math.pow(goal_y - player1.getY(), 2))
        output_distance = pidDistance.update(0, distance, chrono.get_millisecond())

        perc = randint(-100, 100)
        perc = perc/20

        output_distance = output_distance + perc*output_distance

        plot_time.append(chrono.get_millisecond() + 1000*chrono.get_second())
        plot_error.append(output_distance)
        f.write(str(chrono.get_millisecond() + 1000*chrono.get_second()) + ', ' + str(output_distance) + '\n')
        #worksheet.write(row, col, str(chrono.get_millisecond() + 1000*chrono.get_second()))
        #worksheet.write(row, col+1, str(output_distance))

        row = row+1
       	x1 = -output_distance * math.cos(goal_theta)
        y1 = output_distance * math.sin(goal_theta)

        #print(output_distance, file=sys.stderr)
        keys = pygame.key.get_pressed()
        if keys[ K_LEFT]: x1 = -5.0    
        elif keys[ K_RIGHT]: x1 = 5.0 
        #else: x1 = 0.0                  
        if keys[ K_UP]: y1 = -5.0          
        elif keys[ K_DOWN]: y1 = 5.0       
        #else: y1 = 0.0     
        if keys[ K_a]:
            player1.rotate(5)    
        elif keys[ K_d]:
            player1.rotate(-5)

        table_noiseX = randint(-100, 100)
        table_noiseX = table_noiseX/10
        table_noiseY = randint(-100, 100)
        table_noiseY = table_noiseY/10

        x1+=table_noiseX
        y1+=table_noiseY


        #else: x2 = 0.0                  
        #if keys[ K_w]: y2 = -1.0          
        #elif keys[ K_s]: y2 = 1.0       
        #else: y2 = 0.0
        if keys[ K_SPACE]:
            end_game()
            break
        if chrono.get_second() == 25:
            end_game()
            break

        dt = clock.tick(FPS)

        chrono.add_millisecond(dt)

        #if chrono.get_second() >= time:
             #end_game()
             #break

        player1.mod(x1,y1,dt)

        player1.move(dt)


        chrono.blit(screen,font1)
        #disc.blit(screen)
        player1.blit(screen,font1)
        #player2.blit(screen,font1)
        pygame.display.update()

    f.close()
    #workbook.close()
      

def end_game():

    #end_sound.play()
    init_graph()
    #if player1.get_point()>player2.get_point():
     #   end = WIN1
     #   end_label = font2.render(end, 1, (255,0,0))
    #else:
    debug()
    end = WIN2
    end_label = font2.render(end, 1, (0,0,255))

    screen.blit(end_label,(WIDTH*0.5-end_label.get_width()*0.5,HEIGHT*0.5))
    pygame.display.update()
    pygame.time.wait(3000)     
    clock.tick(FPS)    


def instruction_screen():

    while 1:
        instr1_label = font1.render(INSTR1, 1, (0,0,0))
        instr2_label = font1.render(INSTR2, 1, (0,0,0))
        instr3_label = font1.render(INSTR3, 1, (0,0,0))
        instr4_label = font1.render(INSTR4, 1, (0,0,0))
        
        screen.blit( bg_instruction, (0,0))
        screen.blit(instr1_label,(WIDTH*0.5-instr1_label.get_width()*0.5,200))
        screen.blit(instr2_label,(WIDTH*0.5-instr2_label.get_width()*0.5,250))
        screen.blit(instr3_label,(WIDTH*0.5-instr3_label.get_width()*0.5,450))
        screen.blit(instr4_label,(WIDTH*0.5-instr4_label.get_width()*0.5,500))
        
        pygame.display.update()
        for event in pygame.event.get():
            if event.type == QUIT:
                sys.exit()
      
        keys = pygame.key.get_pressed()
        if keys[ K_ESCAPE]:
            break
        

plot_time = []
plot_error = []

#kp, ki, kd, input range, output range, angle boolean
pidDistance = pid_controller.PIDController(.1, .1, .1, [-1, 1], [-200, 200], is_angle=False)
pidTheta = pid_controller.PIDController(1, 1, 1, [10, 10], [-200, 200], is_angle=True)
pygame.init()

mixer = pygame.mixer
mixer.init()
collision_sound=mixer.Sound(COLLISION_SOUND)
#end_sound=mixer.Sound(END_SOUND)
#goal_sound=mixer.Sound(GOAL_SOUND)

os.environ['SDL_VIDEO_CENTERED'] = '1'

icon = pygame.image.load(ICON)
pygame.display.set_icon(icon)
pygame.display.set_caption(TITLE)

screen = pygame.display.set_mode((WIDTH,HEIGHT), 0, 32)

bg = pygame.image.load(BG_PATH).convert()
bg_instruction = pygame.image.load(BG_INSTRUCTION_PATH).convert()

clock = pygame.time.Clock()
chrono = Chrono(0,0,0)



player1 = Mallet(PLAYER1_START,MALLET_MASS,MALLET_RED_PATH,MALLET_MAX_SPEED,MALLET_ACCELERATION,MALLET_FRICTION,1)


posts = []
posts.append(Goalpost((BORDER_LEFT,GOAL_START), (-1,-1), GOALPOST_MASS, GOALPOST_PATH))
posts.append(Goalpost((BORDER_LEFT,GOAL_END), (-1,1), GOALPOST_MASS, GOALPOST_PATH))
posts.append(Goalpost((WIDTH-BORDER_RIGHT,GOAL_START), (1,-1), GOALPOST_MASS, GOALPOST_PATH))
posts.append(Goalpost((WIDTH-BORDER_RIGHT,GOAL_END), (1,1), GOALPOST_MASS, GOALPOST_PATH))

font1 = pygame.font.SysFont("Verdana", 24,True)
font2 = pygame.font.SysFont("Verdana", 40,True)

title_game = TITLE.upper()
title_game_label = font2.render(title_game, 1, (0,0,0))
select_time = SELECT_TIME
select_time_label = font1.render(select_time, 1, (0,0,0))
instruction = INSTRUCTION
instruction_label = font1.render(instruction, 1, (0,0,0))
    
while 1:
    screen.blit( bg, (0,0))
    screen.blit(title_game_label,(WIDTH*0.5-title_game_label.get_width()*0.5,200))
    screen.blit(select_time_label,(WIDTH*0.5-select_time_label.get_width()*0.5,300))
    screen.blit(instruction_label,(WIDTH*0.5-instruction_label.get_width()*0.5,400))
    pygame.display.update()
    
    for event in pygame.event.get():
        if event.type == QUIT:
            sys.exit()
      
    keys = pygame.key.get_pressed()
    
    dt = clock.tick(FPS)
    
    time = 0
    
    if keys[ K_1]: time = 15
    elif keys[ K_3]: time = 3
    elif keys[ K_5]: time = 5
    if time>0:
         game()
    elif keys[ K_F1]:
         instruction_screen()
                    