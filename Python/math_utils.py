# -*- coding: utf-8 -*-
import numpy as np
from geometry_util import get_frustum_points_of_box2d



if __name__ == '__main__':
    P = np.asarray([721.538, 0, 609.559, 44.8573, 0, 721.538, 172.854, 0.216379, 0, 0, 1, 0.00274588]).reshape((3, 4))
    dpi = 8000
    DPI = np.diag([1/dpi, 1/dpi, 1])
    #  P = np.dot(DPI, P)
    #  print(generate_parametersv2(P))
    box = [388.75337290879656, 130.2439024390244, 520.79546681057741, 241.46341463414635]
    planes = get_frustum_points_of_box2d(P, box)
    for plane in planes:
        print(plane)
