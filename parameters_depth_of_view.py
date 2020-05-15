import math


def obtain_d1_d2_from_parameters():
    d = 0.01
    r = 8.4

    f = 7
    n = 7

    c = 0.001

    if (c * d + c * r + c * f - d - r) != 0:
        d1 = (d * r + (r ** 2) + d * f + r * f - c * d * r - c * (r ** 2) - c * r * f) / (c * d + c * r + c * f - d - r)
        print("D1:", d1)
        if (f * n - f * d1 - f * r + n * d1 + n * r) != 0:
            d2 = (-f * n * d1 - 2 * f * n * r + f * d1 * r + f * (r ** 2) - n * d1 * r - n * (r ** 2)) / (
                    f * n - f * d1 - f * r + n * d1 + n * r)
            print("D2:", d2)
        else:
            print("Operation can't be done: D2")
    else:
        print("Operation can't be done: D1")


def obtain_f_n_from_parameters():
    d = 0.01
    r = 8.4

    f = 7
    n = 7

    c = 0.001

    d1 = (d * r + (r ** 2) + d * f + r * f - c * d * r - c * (r ** 2) - c * r * f) / (c * d + c * r + c * f - d - r)
    print("D1:", d1)

    d2 = (-f * n * d1 - 2 * f * n * r + f * d1 * r + f * (r ** 2) - n * d1 * r - n * (r ** 2)) / (
            f * n - f * d1 - f * r + n * d1 + n * r)
    print("D2:", d2)


def obtain_camera_to():
    o1 = (1, 0.4)
    o3 = (o1[0] - math.sin(math.radians(80)), o1[1] - math.cos(math.radians(80)))
    print(o3)


if __name__ == '__main__':
    obtain_camera_to()
