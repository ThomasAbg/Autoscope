//color("yellow")
//bottom circle
/*    rotate_extrude($fn=100)
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
*/

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


//supports
	rotate([0, 0, 90])
	translate([9, 18, 34])
	cube([11, 4, 8]);
	rotate([0, 0, 270])
	translate([9, 18, 34])
	cube([11, 4, 8]);
	rotate([0, 0, 90])
	translate([-9-11, 18, 34])
	cube([11, 4, 8]);
	rotate([0, 0, 270])
	translate([-9-11, 18, 34])
	cube([11, 4, 8]);
//bows
	rotate([0, 90, 0])
	translate([-66, 0, 19])
	difference() {
		cylinder(2, 30, 30, $fn=100);
		translate([-20, -10, -1])
		cylinder(4, 35, 35, $fn=100);
		}
	rotate([0, 90, 180])
	translate([-66, 0, 19])
	difference() {
		cylinder(2, 30, 30, $fn=100);
		translate([-20, 10, -1])
		cylinder(4, 35, 35, $fn=100);
		}
//motor
color("grey")
    translate([-4, 20, 34])
    cube(8);
color("grey")
    translate([4, 24, 38])
	rotate([0, 90, 0])
    cylinder(4, 0.5, 0.5, $fn=100);
color("grey")
    translate([8, 24, 38])
	rotate([0, 90, 0])
    cylinder(2, 3, 3, $fn=100);
//wheel
color("grey")
	rotate([0, 0, 180])
	translate([-19, 16, 38])
	rotate([0, 90, 0])
    cylinder(38, 0.5, 0.5, $fn=100);
color("grey")
    translate([8, -16, 38])
	rotate([0, 90, 0])
    cylinder(2, 3, 3, $fn=100);
//axis
color("grey")
	translate([-19, 26, 76])
	rotate([0, 90, 0])
    cylinder(38, 0.5, 0.5, $fn=100);
color("grey")
	rotate([0, 0, 180])
	translate([-19, 24, 52])
	rotate([0, 90, 0])
    cylinder(38, 0.5, 0.5, $fn=100);
//belt
color("grey")
	translate([8, -16, 35])
	cube([2, 40, 0.5]);
color("grey")
	translate([10, -19, 37.3])
	rotate([69, 0, 180])
	cube([2, 15.5, 0.5]);
color("grey")
	translate([8, 27, 37.3])
	rotate([90.7, 0, 0])
	cube([2, 39, 0.5]);
//sensor
color("grey")
    translate([-18, 14, 39])
    cube([2, 8, 3]);
color("grey")
    translate([-18, 21, 42])
    rotate([0, -30, -90])
    cube([8, 2, 0.2]);
//sensor
color("grey")
    translate([-18, -22, 39])
    cube([2, 8, 3]);
color("grey")
    translate([-16, -21, 42])
    rotate([0, -30, 90])
    cube([8, 2, 0.2]);