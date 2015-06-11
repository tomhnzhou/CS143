-- 1)
-- names of all the actors in the movie 'Die Another Day'
select concat_ws(' ', a.first, a.last) as Names
from Actor a, MovieActor ma, Movie m 
where a.id = ma.aid and
	m.id = ma.mid and
	m.title = 'Die Another Day';

-- 2)
-- count of all the actors who acted in multiple movies
select count(*) as NumActors
from (select aid
		from MovieActor
		group by aid
		having count(mid) > 1) as blah;

-- 3)
-- number of Directors who had direct Crime movie
select count(md.did) as NumDirectors
from MovieDirector md, MovieGenre mg
where md.mid = mg.mid and
		mg.genre = 'Crime';

-- 4)
-- Movie id , Movie title and the number of actors in every movie directed by Ang Lee
select m1.id as MovieID, m1.title as MovieTitle, count(ma.aid) as NumberOfActors
from MovieActor ma join Movie m1 on ma.mid = m1.id
where m1.id in
	(select m.id
	from Movie m, MovieDirector md, Director d
	where m.id = md.mid and
		md.did = d.id and
		d.first = 'Ang' and
		d.last = 'Lee')  
group by m1.id;

-- 5)
-- Director names and number of Comedy films they have directed (if any)
select d.first as FirstName, d.last as LastName, count(md.mid) as NumComedy
from Director d join MovieDirector md on d.id = md.did 
where md.mid in (select mg.mid 
				from MovieGenre mg 
				where mg.genre = 'Comedy')
group by d.id
order by count(md.mid) desc;

