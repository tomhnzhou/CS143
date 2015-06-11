create table Movie (
	id		int primary key, 
		-- Every movie must have a unique id number
	title	varchar(100) not null,
	year	int,
	rating	varchar(10),
	company	varchar(50)
	) ENGINE=INNODB;

create table Actor (
	id		int primary key,
		-- Every actor must have a unique id number
	last	varchar(20) not null,
	first	varchar(20) not null,
	sex		varchar(6) not null,
	dob		date not null, 
	dod		date
	) ENGINE=INNODB;

create table Director (
	id		int primary key, 
		-- Every director must have a unique id number
	last	varchar(20) not null,
	first	varchar(20) not null,
	dob		date not null, 
	dod		date
	) ENGINE=INNODB;

create table MovieGenre (
	mid		int,
	genre  	varchar(20), 
	foreign key (mid) references Movie(id)
		-- Every movie id in this table must be already in Movie table
	) ENGINE=INNODB;

create table MovieDirector (
	mid 	int,
	did		int,
	foreign key (mid) references Movie(id),
		-- Every movie id in this table must be already in Movie table
	foreign key (did) references Director(id)
		-- Every actor id in this table must be already in Actor table
	) ENGINE=INNODB;

create table MovieActor (
	mid 	int,
	aid		int,
	role	varchar(50),
	foreign key (mid) references Movie(id),
		-- Every movie id in this table must be already in Movie table
	foreign key (aid) references Actor(id)
		-- Every actor id in this table must be already in Actor table
	) ENGINE=INNODB;

create table Review (
	name	varchar(20),
	time	timestamp,
	mid 	int,
	rating	int,
	comment	varchar(500),
	foreign key (mid) references Movie(id)
		-- Every movie id in this table must be already in Movie table
	) ENGINE=INNODB;

create table MaxPersonID (
	id		int primary key);

create table MaxMovieID (
	id 		int primary key);
