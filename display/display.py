import curses
import serial
import time
import sys
import logging

data = [-1] * 9
pos = [0, 0, 0]
vel = [0, 0, 0]

logging.basicConfig(
    format='%(asctime)s %(levelname)-8s %(message)s',
    level=logging.INFO,
    datefmt='%Y-%m-%d %H:%M:%S',
    filename=f'rocket_launch_{time.strftime("%H_%M_%S", time.localtime())}.log')

dt = 10/1000.0
#ACCEL_VEL_TRANSITION =  10/1000.0;
#ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;

def update(stdscr, ser):
    global data, vel, pos
    out = str(ser.readline(), 'utf-8', 'ignore').split(' ')
    data = [float(out[i]) for i in range(9)]

    logging.info(str(data))
    
    vel = [a + (round(b, 0) * dt) for a, b in zip(vel, data[:3])]
    pos = [a + (b * dt) for a, b in zip(pos, vel)]
    stdscr.addstr(20, 5, time.strftime("%H:%M:%S", time.localtime()))
    stdscr.addstr(24, 5, f"X: {vel[0]}")
    stdscr.addstr(25, 5, f"Y: {vel[1]}")
    stdscr.addstr(26, 5, f"Z: {vel[2]}")
    stdscr.addstr(30, 5, f"X: {pos[0]}")
    stdscr.addstr(31, 5, f"Y: {pos[1]}")
    stdscr.addstr(32, 5, f"Z: {pos[2]}")

def print_heading(stdscr, x, y, text):
    stdscr.attron(curses.color_pair(1))
    stdscr.addstr(x, y, text)
    stdscr.attroff(curses.color_pair(1))

def main(stdscr):
    k = 0
    ser = serial.Serial(sys.argv[1], 9600, timeout=1)
    time.sleep(1)
    statusbarstr = "Press 'q' to exit"

    curses.start_color()
    curses.init_pair(1, curses.COLOR_CYAN, curses.COLOR_BLACK)
    curses.init_pair(2, curses.COLOR_RED, curses.COLOR_BLACK)
    curses.init_pair(3, curses.COLOR_GREEN, curses.COLOR_BLACK)
    curses.init_pair(4, curses.COLOR_BLACK, curses.COLOR_WHITE)

    stdscr.nodelay(True)

    while (k != ord('q')):
        stdscr.clear()
        height, width = stdscr.getmaxyx()
        try:
            update(stdscr, ser)
        except Exception as e:
            #print(e)
            pass

        print_heading(stdscr, 0, 0, "Calibration")
        stdscr.addstr(2, 5, f"Accelerometer: {data[8]}")
        stdscr.addstr(3, 5, f"    Gyroscope: {data[7]}")

        print_heading(stdscr, 5, 0, "Acceleration")
        stdscr.addstr(7, 5, f"X: {data[0]}")
        stdscr.addstr(8, 5, f"Y: {data[1]}")
        stdscr.addstr(9, 5, f"Z: {data[2]}")

        print_heading(stdscr, 11, 0, "Quaternion")
        stdscr.addstr(13, 5, f"qW: {data[3]}")
        stdscr.addstr(14, 5, f"qX: {data[4]}")
        stdscr.addstr(15, 5, f"qY: {data[5]}")
        stdscr.addstr(16, 5, f"qZ: {data[6]}")

        print_heading(stdscr, 18, 0, "Data")
        print_heading(stdscr, 22, 5, "Velocity")
        print_heading(stdscr, 28, 5, "Displacement")

        stdscr.attron(curses.color_pair(4))
        stdscr.addstr(height-1, 0, statusbarstr)
        stdscr.addstr(height-1, len(statusbarstr), " " * (width - len(statusbarstr) - 1))
        stdscr.attroff(curses.color_pair(4))

        stdscr.refresh()
        k = stdscr.getch()

curses.wrapper(main)
