# Autoscope

Autoscope is a 3D printed Dobson telescope automated by a Raspberry-Pi 3.

## Dependent repositories

The Yocto layer to build the telecope OS :

- https://github.com/thomaslepoix/meta-autoscope

The Stellarium plugin to control the telescope :

- https://github.com/thibaudledo/Autoscope-Stellarium-plugin

<br>

## Branch organization

- master : Readme & alpha release
- hardware : 3D blender files & PCB kicad files
- doc : Documentation, datasheets, etc.
- latex : Latex documents
- hello_mod : Exemple, helloworld kernel module
- mpu_9250\_mod : IMU driver
- a4988_mod : Motors & position sensors driver
- mtk3339_mod : GPS driver

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


