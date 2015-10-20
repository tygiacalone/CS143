

/*  Find the names of all the actors in the movie 'Die Another Day'. 
Please also make sure actor names are in this format:  <firstname> <lastname>   
(seperated by single space). */
SELECT CONCAT (first, ' ', last) AS "Actors in Die Another Day"
FROM Actor
JOIN MovieActor ON Actor.id = MovieActor.aid
JOIN Movie ON MovieActor.mid = Movie.id
WHERE Movie.title = 'Die Another Day';


/*Find the count of all the actors who acted in multiple movies. */
SELECT SUM(count1) AS "Total Actors in Multiple Movies" FROM (
	SELECT COUNT(*) AS count1
	FROM MovieActor
	GROUP BY aid
	HAVING COUNT(*) >= 2 
) a;

/*
SELECT COUNT(*)
FROM (
	SELECT Actor.id
	FROM Actor
	JOIN MovieActor ON Actor.id = MovieActor.aid
	GROUP BY Actor.id
	HAVING COUNT(MovieActor.aid) > 1
) as tmp;

SELECT COUNT(id)
FROM Actor;
*/

/* Return the title(s) of all New Light Films that are rated R*/
SELECT title
FROM Movie
WHERE rating = 'R' AND company = 'New Light Films';
