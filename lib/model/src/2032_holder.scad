$fn = 64;

difference()
{
	union()
	{
		cylinder(d = 22.1, h = 7.5);
		translate([13.45-4,-7.5/2,0]) cube([4,7.5,7.5]);
	}
	translate([0,0,1]) cylinder(d = 20.1, h = 10);

	hull()
	{
		translate([-6, -20, 4.3]) cube([12, 40, 0.1]);
		translate([-9, -20, 7.51]) cube([18, 40, 0.1]);
	}
	translate([13.45-5,-5.5/2,1]) cube([4,5.5,8.5]);
}

translate([-7.90,0,-3.5/2]) cube([0.5,0.7,3.5], center = true);
translate([12.1,0,-3.5/2]) cube([0.5,0.7,3.5], center = true);