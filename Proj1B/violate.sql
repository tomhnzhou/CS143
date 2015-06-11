-- ===========
--	Movie
-- ===========

-- Add a movie with an already-existing id:
-- Violates: every movie must has a unique id
--			There is already a movie with id = 2
-- MySQL Output: Duplicate entry '2' for key 1
insert into Movie values(2, 'Some Movie', 2014, 'R', 'Some Company');

-- Add a movie with a NULL title
-- Violates: every movie must have a valid title that is not null
-- MySQL Output: Column 'title' cannot be null
insert into Movie values(4750, null, 2014, 'R', 'Some Company');

-- ===========
--	Actor
-- ===========

-- Add an actor with an already-existing id
-- Violates: every actor must has a unique id
--				There is already an actor with id = 1000
-- MySQL Output: Duplicate entry '1000' for key 1
insert into Actor values(1000, 'Last', 'First', 'Male', '1990-1-1', null);

-- Add an actor with NULL name
-- Violates: every actor must have a valid name that is not null
-- MySQL Output: Column 'last' cannot be null
insert into Actor values(2, null, null, 'Male', '1990-1-1', null);

-- Add an actor with NULL gender
-- Violates: every actor must have a valid gender that is not null
-- MySQL Output: Column 'sex' cannot be null
insert into Actor values(2, 'Last', 'First', null, '1990-1-1', null);

-- Add an actor with NULL date of birth
-- Violates: every actor must have a valid date of birth that is not null
-- MySQL Output: Column 'dob' cannot be null
insert into Actor values(2, 'Last', 'First', 'Male', null, null);

-- ===========
--	Director
-- ===========

-- Add an director with an already-existing id
-- Violates: every director must has a unique id
--				There is already a director with id = 16
-- MySQL: Duplicate entry '16' for key 1
insert into Director values(16, 'Last', 'First', '1990-1-1', null);

-- Add an director with NULL name
-- Violates: every director must have a valid name that is not null
-- MySQL Output: Column 'last' cannot be null
insert into Director values(1000, null, null, '1990-1-1', null);

-- Add an director with NULL date of birth
-- Violates: every director must have a valid date of birth that is not null
-- MySQL Output: Column 'dob' cannot be null
insert into Director values(1000, 'Last', 'First', null, null);

-- ===========
-- MovieGenre
-- ===========

-- Add a movie which is not in table Movie
-- Violates: every movie id in this table must 
-- 				reference a movie id in Movie
-- MySQL Output: Cannot add or update a child row: 
--				a foreign key constraint fails 
--				(`CS143/MovieGenre`, CONSTRAINT `MovieGenre_ibfk_1` 
--				FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
insert into MovieGenre values(5000, 'Crime');

-- ===========
-- MovieDirector
-- ===========

-- Add a movie which is not in table Movie
-- Violates: every movie id in this table must 
-- 				reference a movie id in Movie
-- MySQL Output: Cannot add or update a child row: 
--				a foreign key constraint fails 
--				(`CS143/MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` 
--				FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
insert into MovieDirector values(5000, 1);

-- Add a director which is not in table Director
-- Violates: every director id in this table must 
-- 				reference a director id in Director
-- MySQL Output: Cannot add or update a child row: 
--				a foreign key constraint fails 
--				(`CS143/MovieDirector`, CONSTRAINT `MovieDirector_ibfk_1` 
--				FOREIGN KEY (`did`) REFERENCES `Director` (`id`))
insert into MovieDirector values(1, 70000);

-- ===========
-- MovieActor
-- ===========

-- Add a movie which is not in table Movie
-- Violates: every movie id in this table must 
-- 				reference a movie id in Movie
-- MySQL Output: Cannot add or update a child row: 
--				a foreign key constraint fails 
--				(`CS143/MovieActor`, CONSTRAINT `MovieActor_ibfk_1` 
--				FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
insert into MovieActor values(5000, 1);

-- Add an actor which is not in table Actor
-- Violates: every actor id in this table must 
-- 				reference an actor id in Actor
-- MySQL Output: Cannot add or update a child row: 
--				a foreign key constraint fails 
--				(`CS143/MovieActor`, CONSTRAINT `MovieActor_ibfk_1` 
--				FOREIGN KEY (`aid`) REFERENCES `Actor` (`id`))
insert into MovieActor values(1, 70000);

-- ===========
-- Review
-- ===========

-- Add a movie which is not in table Movie
-- Violates: every movie id in this table must 
-- 				reference a movie id in Movie
-- MySQL Output: Cannot add or update a child row: 
--				a foreign key constraint fails 
--				(`CS143/Review`, CONSTRAINT `Review_ibfk_1` 
--				FOREIGN KEY (`mid`) REFERENCES `Movie` (`id`))
insert into Review values(null, null, 5000, 1, null);