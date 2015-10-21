
CREATE TABLE Movie (
       id int NOT NULL,
       title varchar(100) NOT NULL,
       year int,
       rating varchar(10),
       company varchar(50),
       /* id is primary key */
       PRIMARY KEY(id),
       /* Every movie must have a title */
       CHECK (title IS NOT NULL AND LENGTH(title) > 0),
       /* Every movie must have a release year */
       CHECK (year IS NOT NULL),

);

CREATE TABLE Actor (
       id int NOT NULL,
       last varchar(20),
       first varchar(20),
       sex varchar(6),
       dob date,
       dod date,
       /* id is primary key */
       PRIMARY KEY(id),
       /* Every actor must be born before he/she dies */
       CHECK (dob < dod),
       /* Every actor must have a birth date */
       CHECK (dob IS NOT NULL)

         CHECK (  0 < SELECT COUNT(*)
                     WHERE EXISTS (SELECT id
                                   FROM Director d
                                   JOIN ACtor a
                                   WHERE d.id = a.id AND NOT (d.last = a.last AND d.first = a.first AND d.dob = a.dob AND d.dod = a.dod)
                                  )
               )
);

CREATE TABLE Director (
       id int NOT NULL,
       last varchar(20),
       first varchar(20),
       dob date,
       dod date,
       /* id is primary key */
       PRIMARY KEY(id),
       /* Every director must be born before he/she dies */
       CHECK (dob < dod),
       /* Every director must have a birth date */
       CHECK (dob IS NOT NULL)
);

CREATE TABLE MovieGenre (
       mid int,
       genre varchar(20),
        /* Every movie must be in database */
       FOREIGN KEY (mid) references Movie(id)
) ENGINE=INNODB;;

CREATE TABLE MovieDirector (
       mid int,
       did int,
       /* Every movie and director must be in database */
       FOREIGN KEY (mid) references Movie(id),
       FOREIGN KEY (did) references Director(id)
) ENGINE=INNODB;

CREATE TABLE MovieActor (
       mid int,
       aid int,
       role varchar(50),
       /* Every movie and actor must be in database */
       FOREIGN KEY (mid) references Movie(id),
       FOREIGN KEY (aid) references Actor(id)
) ENGINE=INNODB;

CREATE TABLE Review (
       name varchar(20),
       time timestamp,
       mid int,
       rating int,
       comment varchar(500),
       /* Every movie must be in database */
       FOREIGN KEY (mid) references Movie(id)
) ENGINE=INNODB;

CREATE TABLE MaxPersonID (
       id int
);

CREATE TABLE MaxMovieID (
       id int
);
