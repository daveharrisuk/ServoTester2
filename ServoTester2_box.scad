/* ServoTester2_box.scad */
/* © Dave Harris July 2023 */


pcbX = 50;       /* X PCB */
pcbY = 33.3333;  /* Y PCB */
pcbZ = 1.6;      /* PCB thickness */

pcbHoleY = 26.9; /* Y hole spacing */
pcbHoleX = 43.6; /* X hole spacing */
pcbHoleR = 1.6;  /* M3 clearance */

cornerR = 2.5;   /* box corner rad */

boxWall = 1.1;   /* box wall thickness */

pillarR = 2.5;   /* screw pillar rad */
screwR = 0.6;    /* self tap hole */

component = 16;  /* component depth */

boxH = pcbZ +component +boxWall; /* height */


$fn=50;
  


ServoTester2_box(); /* self test module */



/* generate box */

module ServoTester2_box()
{
  difference() /* diff1 */
  {
    union() /* union1 */
    {
      difference() /* diff2 = basic box */
      {
        hull() /* outline with round corner */
          pillar4( pX = pcbX - cornerR
                 , pY = pcbY - cornerR
                 , rad = cornerR
                 , hi = boxH );
        
        /* hollow out box */
        translate([ 0, 0, boxWall + boxH/2 ])
            cube([ pcbX - boxWall
                 , pcbY - boxWall
                 , boxH ], center=true);
        
      } /* end diff2 */
      
      difference() /* diff3 = add pillars */
      {
        union()
        {
          pillar4( pcbHoleX
                 , pcbHoleY
                 , rad = pillarR );
          
          /* infill gaps */
          pillar4( pcbHoleX +1.5
                  , pcbHoleY +1.5
                  , rad = 3.2 );
        }
        /* cut mount screw holes */
        translate([ 0, 0, boxWall ])
          pillar4( pcbHoleX
                 , pcbHoleY
                 , rad = screwR
                 , hi = 20 );
       
      } /* end diff3 */

    } /* end union1 */
    
    /* cutout PCB rebate */
    translate([ 0
              , 0
              , boxWall + component + pcbZ ])
      pcb();
    
    /* cut out USB socket */
    translate([ -28, -0.4, boxH -4.35 ])
      cube([5, 8.7, 4.362]);
    
    translate([-23,-1,boxWall-0.9])
      linear_extrude(1)
        text("ServoTester2 D.Harris'23",3);
    
  } /* end diff1 */
  
  /* illustrate PCB */
  //color("blue") translate([0, 0, 26]) pcb();
  
} /* end module */



/* generate 4 corner pillars */

module pillar4( pX, pY, rad, hi = boxH )
{
  for( x = [-0.5 : 1: 0.5])
  {
    for( y = [-0.5 : 1 : 0.5])
    {
      translate([ x*(pX), y*(pY), 0 ])
       cylinder( h = hi, r = rad );
    }  
  }
} /* end module */



/* generate PCB */

module pcb()
{
  difference()
  {
    cube([pcbX +0.3, pcbY +0.3, pcbZ]
         , center=true);
    translate([0, 0, -1 ])
      pillar4(pX = pcbHoleX
            , pY = pcbHoleY
            , rad = 0.5 );
  }
} /* end module */

 /* end file */