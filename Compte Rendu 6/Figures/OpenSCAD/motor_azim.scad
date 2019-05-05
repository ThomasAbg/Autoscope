//color("yellow")
//bottom circle
    rotate_extrude($fn=100)
        translate([20, 0])
        square([10, 4]);
//belt
color("grey")
    rotate_extrude($fn=100)
        translate([19, 0])
        square([1, 4]);
//belt notches
color("grey")
    for(i=[0:10:360])
        rotate(i)
        translate([18, 0])
        cube([1, 1, 4]);
//sensor trigger
color("grey")
    translate([0, 30, 2])
    rotate([90, 0, 180])
    cylinder(4, 0.5, 0.5, $fn=100);


//top circle
    rotate_extrude($fn=100)
        translate([20, 30])
        square([10, 4]);
//motor
color("grey")
    translate([-4, 12, 30])
    cube(8);
//motor axis
color("grey")
    translate([0, 16, 24])
    cylinder(6, 0.5, 0.5, $fn=100);
//motor wheel
color("grey")
    translate([0, 16, 24])
    cylinder(2, 3, 3, $fn=100);
//sensor
color("grey")
    translate([-4, 30, 30])
    cube([8, 2, 3]);
color("grey")
    translate([-3, 30, 30])
    rotate([0, 30, 0])
    cube([6, 2, 0.2]);
color("grey")
    translate([-3, 30, 30])
    rotate([0, 30, 0])
    translate([6, 0, 0])
    rotate([0, -60, 0])
    cube([2, 2, 0.2]);