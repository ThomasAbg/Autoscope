# Autoscope

Autoscope is a 3D printed Dobson telescope automated by a Raspberry-Pi 3.

## Dependent repositories

The Yocto layer to build the telecope OS :

- https://github.com/thomaslepoix/meta-autoscope

The Stellarium plugin to control the telescope :

- https://github.com/thibaudledo/Autoscope-Stellarium-plugin

<br>

## Branch & release organization

- [branch : master](https://github.com/thibaudledo/Autoscope/tree/master) : Readme
- [release : alpha](https://github.com/thibaudledo/Autoscope/releases) : Out of the box packages (Stellarium, OS)
- [branch : hardware](https://github.com/thibaudledo/Autoscope/tree/hardware) : 3D blender files & PCB kicad files
- [branch : doc](https://github.com/thibaudledo/Autoscope/tree/doc) : Documentation, datasheets, etc.
- [branch : latex](https://github.com/thibaudledo/Autoscope/tree/latex) : Latex documents
- [branch : hello_mod](https://github.com/thibaudledo/Autoscope/tree/hello_mod) : Exemple, helloworld kernel module
- [branch : mpu_9250_mod](https://github.com/thibaudledo/Autoscope/tree/mpu_9250_mod) : IMU driver
- [branch : a4988_mod](https://github.com/thibaudledo/Autoscope/tree/a4988_mod) : Motors & position sensors driver
- [branch : mtk3339_mod](https://github.com/thibaudledo/Autoscope/tree/mtk3339_mod) : GPS driver

<br>

## Download & work on this repository

```
mkdir Autoscope/
cd Autoscope/
git clone https://github.com/thibaudledo/Autoscope -b master Master
git clone https://github.com/thibaudledo/Autoscope -b doc Documentation
git clone https://github.com/thibaudledo/Autoscope -b latex Latex
git clone https://github.com/thibaudledo/Autoscope -b hardware Hardware

git clone https://github.com/thibaudledo/Autoscope -b hello_mod hello-mod
git clone https://github.com/thibaudledo/Autoscope -b mpu_9250_mod mpu-9250-mod
git clone https://github.com/thibaudledo/Autoscope -b a4988_mod a4988-mod
git clone https://github.com/thibaudledo/Autoscope -b mtk3339_mod mtk3339-mod
```

<br>

## Authors

- Thibaud Le Doledec `<thibaudledo@gmail.com>`
- Thomas Lepoix `<thomas.lepoix@gmail.com>`
- Thomas Abgrall `<01thomas.abgrall@gmail.com>`
- Clément Ailloud `<clem.ailloud@gmail.com>`

<br>

## Inspired by

- https://blog.dagoma.fr/telescope-imprime-en-3d/


