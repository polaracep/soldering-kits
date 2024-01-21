$fn=64;

// Body
color("gray") translate([0,0,3.5/2+0.25]) cube([8.5,3.8,3.0], true);
color("saddlebrown") translate([0,0,0.25]) cube([8.5,3.8,0.5], true);

// Packa
color("black") translate([1,0,3.25]) cube([1.5,1.5,6.49], true);

// Pins
color("silver")
{
	hull()
	{
		translate([2.54,0,-2.54/2]) cube([0.65,0.65,2.54], true);
		translate([2.54,0,-2.84]) cube([0.65/2,0.65/2,0.2], true);
	}

	hull()
	{
		translate([0,0,-2.54/2]) cube([0.65,0.65,2.54], true);
		translate([0,0,-2.84]) cube([0.65/2,0.65/2,0.2], true);
	}

	hull()
	{
		translate([-2.54,0,-2.54/2]) cube([0.65,0.65,2.54], true);
		translate([-2.54,0,-2.84]) cube([0.65/2,0.65/2,0.2], true);
	}
}
