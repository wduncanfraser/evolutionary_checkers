DROP TABLE "Generation_Scores";
DROP TABLE "Network";
DROP TABLE "Generation";
DROP TABLE "Set";

-- Table: "Set"

CREATE TABLE "Set"
(
  id serial NOT NULL,
  name text NOT NULL,
  CONSTRAINT "PK_Set_id" PRIMARY KEY (id),
  CONSTRAINT "UX_Set_Name" UNIQUE (name)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Set"
  OWNER TO checkers;

-- Table: "Generation"


CREATE TABLE "Generation"
(
  id serial NOT NULL,
  "Set_id" integer NOT NULL,
  "Count" integer NOT NULL,
  CONSTRAINT "PK_Generation_id" PRIMARY KEY (id),
  CONSTRAINT "FK_Generation_Set_id" FOREIGN KEY ("Set_id")
      REFERENCES set (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "UX_Set_id_Generation_Count" UNIQUE ("Set_id", "Count")
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Generation"
  OWNER TO checkers;

-- Table: "Network"

CREATE TABLE "Network"
(
  id serial NOT NULL,
  "Set_id" integer NOT NULL,
  count integer NOT NULL,
  "parent_Network_id" integer,
  "origin_Generation_id" integer NOT NULL,
  "death_Generation_id" integer,
  network_blob text NOT NULL,
  CONSTRAINT "PK_Network_id" PRIMARY KEY (id),
  CONSTRAINT "FK_Network_Set_id" FOREIGN KEY ("Set_id")
      REFERENCES set (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "FK_Network_death_Generation_id" FOREIGN KEY ("death_Generation_id")
      REFERENCES "Generation" (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "FK_Network_origin_Generation_id" FOREIGN KEY ("origin_Generation_id")
      REFERENCES "Generation" (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "FK_Network_parent_Network_id" FOREIGN KEY ("parent_Network_id")
      REFERENCES "Network" (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "UX_Set_id_Network_Count" UNIQUE ("Set_id", count)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Network"
  OWNER TO checkers;

-- Table: "Generation_Scores"

CREATE TABLE "Generation_Scores"
(
  "Generation_id" integer NOT NULL,
  "Network_id" integer NOT NULL,
  score integer NOT NULL,
  survived boolean NOT NULL DEFAULT false,
  CONSTRAINT "PK_Generation_Scores" PRIMARY KEY ("Generation_id", "Network_id"),
  CONSTRAINT "FK_Generation_Scores_Generation_id" FOREIGN KEY ("Generation_id")
      REFERENCES "Generation" (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "FK_Generation_Scores_Network_id" FOREIGN KEY ("Network_id")
      REFERENCES "Network" (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "Generation_Scores"
  OWNER TO checkers;
