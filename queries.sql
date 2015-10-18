
SELECT CONCAT (first, ' ', last) AS 'DieAnotherDayActos'
FROM Actor
JOIN MovieActor ON Actor.id = MovieActor.aid
JOIN Movie ON MovieActor.mid = Movie.id
WHERE Movie.title = 'Die Another Day'
