	rotate_extrude($fn=8)
    translate([24, 0])
    square([5, 2]);
	rotate_extrude($fn=8)
    translate([24, 20])
    square([5, 2]);

	rotate([0, 0, 22.5])
	translate([22.5, -10])
	difference() {
		cube([4, 20, 20]);
		translate([-2, 10, 11])
		rotate([0, 90, 0])
		cylinder(10, 8, 8, $fn=100);
		}
	rotate([0, 0, 22.5+90])
	translate([22.5, -10])
	difference() {
		cube([4, 20, 20]);
		translate([-2, 10, 11])
		rotate([0, 90, 0])
		cylinder(10, 8, 8, $fn=100);
		}
	rotate([0, 0, 22.5+180])
	translate([22.5, -10])
	difference() {
		cube([4, 20, 20]);
		translate([-2, 10, 11])
		rotate([0, 90, 0])
		cylinder(10, 8, 8, $fn=100);
		}
	rotate([0, 0, 22.5+270])
	translate([22.5, -10])
	difference() {
		cube([4, 20, 20]);
		translate([-2, 10, 11])
		rotate([0, 90, 0])
		cylinder(10, 8, 8, $fn=100);
		}

	rotate([0, 0, 22.5])
	translate([23, -2])
	cube([55, 4, 2]);
	rotate([0, 0, 22.5])
	translate([22.5, -2, -3])
	cube([2.5, 4, 3]);
	rotate([0, 0, 22.5])
	translate([43, -4, -9])
	cube([10, 8, 19]);

//screw
color("grey")
	rotate([0, 0, 22.5])
	translate([23, 0, -1.5])
	rotate([0, 90, 0])
	cylinder(60, 0.5, 0.5, $fn=100);
color("grey")
	rotate([0, 0, 22.5])
	translate([23, 0, -1.5])
	rotate([0, 90, 0])
	linear_extrude(height = 60, center = false, convexity = 10, twist = -30000)
	translate([0.5, 0, 0])
	circle(r = 0.5, $fn=100);
color("grey")
	rotate([0, 0, 22.5])
	translate([23+60, 0, -1.5])
	rotate([0, 90, 0])
	cylinder(1, 2, 2, $fn=6);

//zoom
color("grey")
	rotate([0, 0, 22.5])
	translate([35, 0, 10])
	rotate([0, 90, 0])
	cylinder(20, 6, 6, $fn=100);
color("grey")
	rotate([0, 0, 22.5])
	translate([55, 0, 10])
	rotate([0, 90, 0])
	cylinder(10, 6, 6, $fn=10);
color("grey")
	rotate([0, 0, 22.5])
	translate([65, 0, 10])
	rotate([0, 90, 0])
	cylinder(10, 6, 6, $fn=100);

//motor
color("grey")
	rotate([0, 0, 22.5])
	translate([43, -2, -13])
	cube([10, 4, 4]);
color("grey")
	rotate([0, 0, 22.5])
	translate([50, 0, -11])
	rotate([0, 90, 0])
	cylinder(12, 0.5, 0.5, $fn=100);
color("grey")
	rotate([0, 0, 22.5])
	translate([60, 0, -11])
	rotate([0, 90, 0])
	cylinder(4, 7, 7, $fn=100);

//belt
color("grey")
	rotate([0, 0, 22.5])
	translate([60, -6.5, -11])
	rotate([87, 0, 0])
	cube([4, 23, 0.5]);
color("grey")
	rotate([0, 0, 22.5])
	translate([60, 7, -11])
	rotate([93, 0, 0])
	cube([4, 23, 0.5]);

//trigger
color("grey")
	rotate([0, 0, 22.5])
	translate([57, 0, 10])
	rotate([30, 0, 0])
	cylinder(20, 0.5, 0.5, $fn=100);
//sensor
color("grey")
	rotate([0, 0, 22.5])
	translate([49, -4, -3])
	rotate([0, 0, -90])
    cube([2, 8, 3]);
color("grey")
	rotate([0, 0, 22.5])
	translate([50, -6, 0])
	rotate([0, 0, -90])
    rotate([0, -30, 90])
    cube([8, 2, 0.2]);