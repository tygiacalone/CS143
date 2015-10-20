/* Primary Key Violations */

/* Violates primary key of Actor because there already exists
   a person with an id of 1 */
INSERT INTO Actor VALUES (1, 'Drazen', 'Chloe', 'female', 02-17-1995, NULL);

/* Violates primary key of Director because there already exists
   a person with an id of 2 */
INSERT INTO Director VALUES (2, 'Giacalone', 'Ty', 05-09-1994, NULL);

/* Violates primary key of Movie because there already exists
   a movie with an id of 12 */
INSERT INTO Movie VALUES (12, "CS143 Is The Best Class Ever", 2015, "PG", "UCLA");


/* Foreign Key Violations */

/* Violate foreign key becaus Movie ID does not exist */
INSERT INTO MovieGenre VALUES (69100, "Horror");

/* Violate foreign key becaus Movie ID does not exist */
INSERT INTO MovieDirector VALUES (69100, 12);

/* Violate foreign key becaus Director ID does not exist */
INSERT INTO MovieDirector VALUES (1, 4770);

/* Violate foreign key becaus Movie ID does not exist */
INSERT INTO MovieActor VALUES (69100, 35, "Wicked Witch");

/* Violate foreign key becaus Actor ID does not exist */
INSERT INTO MovieActor VALUES (72, 4780, "Wicked Witch");

/* Violate foreign key becaus Movie ID does not exist */
INSERT INTO Review VALUES ("Chloe Drazen", '2010-01-01 00:00:01', 69100, 10, "Great movie!");


/* Check Constraint Violations */

/* Violates Check Constraint because Actor's dod is before dob */
INSERT INTO Actor VALUES (4760, 'Drazen', 'Chloe', 'female', 02-17-1995, 01-01-1995);

/* Violates Check Constraint because Actor's dob is NULL*/
INSERT INTO Actor VALUES (4760, 'Drazen', 'Chloe', 'female', NULL, 01-01-1995);

/* Violates Check Constraint because Movie has no title */
INSERT INTO Movie VALUES (69100, NULL, 2015, "PG", "UCLA");

/* Violates Check Constraint because Movie has no release date */
INSERT INTO Movie VALUES (69200, "CS143 Is The Best Class Ever", NULL, "PG", "UCLA");

