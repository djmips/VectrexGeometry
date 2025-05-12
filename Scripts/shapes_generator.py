# %%
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['figure.dpi'] = 200


# %%
def print_lines(file: str, arrayName:str, x1: list, x2: list, y1: list, y2: list, ax=None):
    """ Plots lines defined by x and y coordinates.

    Args:
        file (str): Name of the file
        arrayName (str): Name of the array
        x1 (list): x coordinates of point 1
        x2 (list): x coordinates of point 2
        y1 (list): y coordinates of point 1
        y2 (list): y coordinates of point 2
    """

    with open(file, 'w') as f:

        print ("const signed char %s[] = {" % arrayName, file=f)

        for i in range(np.size(x1)):
            #ax.plot([x1[i], x2[i]], [y1[i], y2[i]], 'C0', linewidth=0.5)
            dx = (x2[i] - x1[i])
            dy = (y2[i] - y1[i])
            print ("%d,%d," % (dx, dy), file=f)

        print ("};", file=f)




"""
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
U = address of vectorlist
X = (y,x) position of vectorlist (this will be point 0,0), positioning on screen
scaleMove = scalefactor "Move" (after sync)
scaleDraw = scalefactor "Draw" (vectors in vectorlist)

const signed char drawList[] = {
     mode, dy, dx,                                             
     mode, dy, dx,                                             
     .      .      .                                                
     mode, dy, dx,                                             
     0x02
     }

 where mode has the following meaning:         
 negative: draw line                    
 $00:        move to specified endpoint     
 $FF:        draw relative line to specified endpoint
 $01:        sync (and move to list start and than to place in vectorlist)      
 $02:        end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
"""

def print_triangles(file: str, arrayName:str, x1: list, x2: list, x3: list, y1: list, y2: list, y3: list, ax=None):
    """ This routine needs to print out a drawList

    Args:
        file (str): Name of the file
        arrayName (str): Name of the array
        x1 (list): x coordinates of point 1
        x2 (list): x coordinates of point 2
        x3 (list): x coordinates of point 3
        y1 (list): y coordinates of point 1
        y2 (list): y coordinates of point 2
        y3 (list): y coordinates of point 3
    """

    with open(file, 'w') as f:

        print ("const signed char %s[] = {" % arrayName, file=f)

        #print the number of triangles in the array
        print ("%d, // Number of triangles" % (np.size(x1)), file=f)
        print ("", file=f)

        for i in reversed(range(np.size(x1))):
            #first print the move location of the first vertex of the triangle
            print ("0,%d,%d, // Move" % (x1[i], y1[i]), file=f)

            #then calculate the delta to the second vertrex and print that
            dx = x2[i] - x1[i]
            dy = y2[i] - y1[i]
            print ("-1,%d,%d, // Line to second vertex" % (dx, dy), file=f)

            #then calculate the delta to the third vertrex and print that
            dx = x3[i] - x2[i]
            dy = y3[i] - y2[i]
            print ("-1,%d,%d, // Line to third vertex" % (dx, dy), file=f)

            #then calculate the delta back to the first vertrex and print that
            dx = x1[i] - x3[i]
            dy = y1[i] - y3[i]
            print ("-1,%d,%d, // Line to first vertex" % (dx, dy), file=f)

            # sync
            print ("1,0,0, // Sync", file=f)

            print ("", file=f)

        # terminate with the literal 2
        print ("2};", file=f)

# %%
def plot_lines(name: str, x1: list, x2: list, y1: list, y2: list, ax=None):
    """ Plots lines defined by x and y coordinates.

    Args:
        name (str): Name of the plot
        x1 (list): x coordinates of point 1
        x2 (list): x coordinates of point 2
        y1 (list): y coordinates of point 1
        y2 (list): y coordinates of point 2
    """

    # Reuse the same axes if specified
    ax = ax or plt.gca()
    ax.set_aspect('equal')

    for i in range(np.size(x1)):
        ax.plot([x1[i], x2[i]], [y1[i], y2[i]], 'C0', linewidth=0.5)

    ax.set_title(name)
    # ax.grid()

def disk():
    """ A very simple disk made with one hundred straight lines.
    """
    t = np.arange(0, 2 * np.pi, np.pi / 50)
    a = t + 3 * np.pi / 4

    x1 = np.cos(t) * 200 + 202
    y1 = np.sin(t) * 200 + 202
    x2 = np.cos(a) * 200 + 202
    y2 = np.sin(a) * 200 + 202

    plot_lines("Disk", x1, x2, y1, y2)
    plt.show()

def box():
    """ Figure completely made of squares.
    """

    t = np.arange(0, 2 * np.pi, np.pi / 30)
    a = t + np.pi / 2

    r1 = np.cos(2*t) * 200
    x1 = np.multiply(np.cos(t), r1) + 202
    y1 = np.multiply(np.sin(t), r1) + 202

    r2 = np.cos(2*a) * 200
    x2 = np.multiply(np.cos(a), r2) + 202
    y2 = np.multiply(np.sin(a), r2) + 202

    plot_lines("Box", x1, x2, y1, y2)
    plt.show()

def outward_triangles():
    """ Figure made of triangles.
    """

    t = np.arange(0, np.pi, np.pi / 40)
    r = t * 60
    a = t + (2 * np.pi / 3)
    b = t + (4 * np.pi / 3)

    x1 = np.cos(t) * r + 202
    y1 = np.sin(t) * r + 202

    x2 = np.cos(a) * r + 202
    y2 = np.sin(a) * r + 202

    plot_lines("Outward Triangles", x1, x2, y1, y2)

    x1 = np.cos(b) * r + 202
    y1 = np.sin(b) * r + 202

    plot_lines("Outward Triangles", x1, x2, y1, y2)

    x2 = np.cos(t) * r + 202
    y2 = np.sin(t) * r + 202

    plot_lines("Outward Triangles", x1, x2, y1, y2)
    plt.show()

def cyclone_spiral():
    """ Overlapping spiral made of 200 lines
    """

    t = np.arange(0, 10 * np.pi, np.pi / 20)
    a = t + 2 * np.pi / 3

    x1 = np.cos(t) * 5 * t + 202
    y1 = np.sin(t) * 5 * t + 202

    x2 = np.cos(a) * 5 * t + 202
    y2 = np.sin(a) * 5 * t + 202

    plot_lines("Spiral", x1, x2, y1, y2)
    plt.show()

def star():
    """ Based on 8 leaf rose
    """

    t = np.arange(0, 2 * np.pi, np.pi / 60)
    r1 = np.cos(4 * t) * 200
    a = t + np.pi / 4
    r2 = np.cos(4 * a) * 202

    x1 = np.cos(t) * r1 + 202
    y1 = np.sin(t) * r1 + 202

    x2 = np.cos(a) * r2 + 202
    y2 = np.sin(a) * r2 + 202

    plot_lines("Star", x1, x2, y1, y2)
    plt.show()

def inward_triangles():
    """ Inward triangles.
    """
    tau = np.pi * 2
    t = np.arange(0, 2.64, np.pi / 22)

    rconst = 1.17557
    
    r = [3*rconst]
    for i in range(len(t)-1):
        r.append(r[i] * rconst)

    a = t + (tau / 3)
    b = t + (2 * tau / 3)

    const = 0

    #calculate the x,y coordinates of the first vertex of all the triangles
    x1 = np.multiply(np.cos(t), r) + const
    y1 = np.multiply(np.sin(t), r) + const

    #calculate the x,y coordinates of the second vertex of all the triangles
    x2 = np.multiply(np.cos(a), r) + const
    y2 = np.multiply(np.sin(a), r) + const

    #calculate the x,y coordinates of the third vertex of all the triangles
    x3 = np.multiply(np.cos(b), r) + const
    y3 = np.multiply(np.sin(b), r) + const

    # pass a file name that's relative to current directory with directory GeometrySource/source
    print_triangles("GeometrySrc/source/triangles.h", "triangles", x1, x2, x3, y1, y2, y3)

    plot_lines("inward", x1,x2,y1,y2)
    plot_lines("inward", x2,x3,y2,y3)
    plot_lines("inward", x3,x1,y3,y1)
    plt.show()

def circle_int_lines():
    """ A circle with internal connecting lines.
    """

    n = 18
    t = np.arange(0, np.tau - 0.001, np.tau / n)

    a = np.cos(t) * 200 + 202
    b = np.sin(t) * 200 + 202

    for i in range(n):
        x1 = np.ones(n-1) * a[i]
        y1 = np.ones(n-1) * b[i]
        x2 = a[1:n]
        y2 = b[1:n]

        plot_lines("Circle Internal Lines", x1, x2, y1, y2)

    plt.show()

def tunnel():
    """ The tunnel is a square spiral.
    """

    x1 = 270
    y1 = 270

    for t in range(64):
        x2 = x1 + 5*t + 2
        y2 = y1

        plot_lines("Tunnel", [x1], [x2], [y1], [y2])

        x1 = x2
        y1 = y2 + (5 * t) + 3

        plot_lines("Tunnel", [x1], [x2], [y1], [y2])

        x2 = x1 - (5 * t) - 5
        y2 = y1

        plot_lines("Tunnel", [x1], [x2], [y1], [y2])

        x1 = x2
        y1 = y2 - (5 * t) - 6

        plot_lines("Tunnel", [x1], [x2], [y1], [y2])

    plt.show()

def moire_pattern():
    """ A Moire pattern created by interference of regulaly spaced radii of two circles.
    """

    t = np.arange(0, np.pi / 2, np.pi / 180)

    x1 = 100 * np.cos(t)
    y1 = 100 * np.sin(t)
    x2 = 405 * np.cos(t)
    y2 = 405 * np.sin(t)

    plot_lines("Moire", x1, x2, y1, y2)

    x1 = 405 - x1
    y1 = 405 - y1
    x2 = 405 - x2
    y2 = 405 - y2

    plot_lines("Moire", x1, x2, y1, y2)
    plt.show()

def four_leaf_rose():
    """ Uses polar coordinates and is based on a four leaf rose.
    """

    t = np.arange(0, 2 * np.pi, np.pi / 75)
    a = t + np.pi / 3
    r1 = 200 * np.cos(2*t)
    r2 = 200 * np.cos(2*a)

    x1 = r1 * np.cos(t) + 202
    y1 = r1 * np.sin(t) + 202
    x2 = r2 * np.cos(a) + 202
    y2 = r2 * np.sin(a) + 202

    plot_lines("Four Leaf Rose", x1, x2, y1, y2)
    plt.show()

def parabola():
    """ parabolic border with circle that has internal connecting lines.
    """

    # Parabolas
    q = np.arange(5, 405, 9)

    x1 = np.ones(len(q)) * 5
    y1 = q
    x2 = q
    y2 = np.ones(len(q)) * 405

    plot_lines("Parabola", x1, x2, y1, y2)

    x1 = q
    y1 = np.ones(len(q)) * 5
    x2 = np.ones(len(q)) * 405
    y2 = q

    plot_lines("Parabola", x1, x2, y1, y2)

    # Circle
    n = 18
    t = np.arange(0, 2 * np.pi - 0.001, 2 * np.pi / n)

    a = np.cos(t) * 125 + 202
    b = np.sin(t) * 125 + 202

    for i in range(n):
        x1 = np.ones(n-1) * a[i]
        y1 = np.ones(n-1) * b[i]
        x2 = a[1:n]
        y2 = b[1:n]

        plot_lines("Parabola", x1, x2, y1, y2)

    plt.show()

# %%
if __name__ == '__main__':
    #disk()
    #box()
    #outward_triangles()
    #cyclone_spiral()
    #star()
    inward_triangles()
    #circle_int_lines()
    #tunnel()
    #moire_pattern()
    #four_leaf_rose()
    #parabola()
# %%
