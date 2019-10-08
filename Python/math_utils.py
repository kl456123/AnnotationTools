# -*- coding: utf-8 -*-
import numpy as np


def decompose_QR():
    pass


def decompose_project_matrix(P):
    """
    M = KR
    P = K[R|T]
      = [M|MT]
    """
    M = P[:3, :3]
    MT = P[:, 3]

    T = np.dot(np.linalg.inv(M), MT)
    K, R = decompose_QR(M)
    return K, R, T


def generate_parametersv1(camera_parameters):
    """
    generate focal_point, position, viewup vector

    KR(X-C) = K(RX+T)
    -RC = T
    """
    K, R, T = camera_parameters
    focal_point = None
    position = None
    view_up = None
    position = -np.dot(np.linalg.inv(R), T)
    return focal_point, position, view_up


def generate_parametersv2(P):
    dpi = 8000
    P
    x_plane = P[1]
    view_up = x_plane[:3]
    M = P[:3, :3]
    position = np.dot(np.linalg.inv(M), P[:, 3])
    z_plane = P[2]
    z_plane[-1] = 0
    focal_point = np.dot(P, z_plane)
    return focal_point, position, view_up/np.linalg.norm(view_up)

def get_frustum_points_of_box2d(P, box):
    # line : (x1, y1) (x2, y2)
    # Ax+By+C = 0
    # A = y2 - y1
    # B = x1 - x2
    # C = x2*y1 - x1 * y2
    l_left = [box[0], box[1], box[0], box[3]]
    l_top = [box[0], box[1], box[2], box[1]]
    l_right = [box[2], box[1], box[2], box[3]]
    l_bottom = [box[0], box[3], box[2], box[3]]
    bounding_planes = []
    for l in [l_left, l_top, l_right, l_bottom]:
        A = l[3] - l[1]
        B = l[0] - l[2]
        C = l[2] * l[1] - l[0] * l[3]
        plane = np.dot(P.T, np.array([A, B, C]))
        normal, origin = GetNormaAndOrigin(plane)
        print(normal, origin)

def GetNormaAndOrigin(plane):
    # plane a,b,c,d
    normal = plane[:3]
    if normal[2]:
        origin = [0, 0, -plane[3] / normal[2]]
    elif normal[1]:
        origin = [0, plane[3] / normal[1], 0]
    else:
        origin = [plane[3] / normal[0], 0, 0]
    return normal, origin


if __name__ == '__main__':
    P = np.asarray([7.070493000000e+02, 0.000000000000e+00, 6.040814000000e+02, 4.575831000000e+01,
                    0.000000000000e+00, 7.070493000000e+02, 1.805066000000e+02, -3.454157000000e-01,
                    0.000000000000e+00, 0.000000000000e+00, 1.000000000000e+00, 4.981016000000e-03]).reshape((3, 4))
    dpi = 8000
    DPI = np.diag([1/dpi, 1/dpi, 1])
    #  P = np.dot(DPI, P)
    #  print(generate_parametersv2(P))
    box = [472.736, 182.812, 509.996 ,210]
    get_frustum_points_of_box2d(P, box)
