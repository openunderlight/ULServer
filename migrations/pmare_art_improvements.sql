DROP PROCEDURE IF EXISTS PMAREARTSIMPROVEMENT;

DELIMITER //

CREATE PROCEDURE ul_player.PMAREARTSIMPROVEMENT() 

BEGIN

  DECLARE playerMareId INT;
  DECLARE playerMareSkill INT;
  DECLARE playerMareSkillLevel INT;
  DECLARE totalArtsInserted INT DEFAULT 0;
  DECLARE done INT DEFAULT 0;
  DECLARE playerMareCursor CURSOR FOR SELECT player_id FROM player WHERE acct_type = 83;
  DECLARE playerMareArtCursor CURSOR FOR SELECT skill, skill_level FROM temp_playermare_arts;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = 1;

  DELETE FROM ul_player.skill WHERE player_id IN (SELECT player_id FROM player WHERE acct_type = 83);

  CREATE TEMPORARY TABLE IF NOT EXISTS temp_playermare_arts (
    skill INT NOT NULL,
    skill_level INT NOT NULL
  );

  INSERT INTO temp_playermare_arts (skill, skill_level) 
  VALUES
     ( 96, 90),  -- Sense Dreamers
     (  6, 19),  -- Meditation
     ( 12, 19),  -- Shatter
     ( 79, 90),  -- Dreamquake
     ( 20, 60),  -- Blast
     (139, 90),  -- Sense Datoken
     ( 24, 19),  -- Restore
     ( 28, 90),  -- Abjure
     ( 29, 60),  -- Poison
     ( 30, 90),  -- Antidote
     ( 32, 29),  -- Drain Essence
     ( 42, 90),  -- Darkness
     ( 44, 60),  -- Firestorm
     ( 82, 90),  -- Terror
     (140, 90)  -- Tempest
  ;

 
  OPEN playerMareCursor;

  player_mare_loop :
    LOOP
      FETCH playerMareCursor INTO playerMareId;
      IF done THEN
	CLOSE playerMareCursor;
        LEAVE player_mare_loop;
      END IF;

    OPEN playerMareArtCursor;

    player_mare_art_loop:
      LOOP
        FETCH playerMareArtCursor INTO playerMareSkill, playerMareSkillLevel;
        IF done THEN
          SET done = 0;
          CLOSE playerMareArtCursor;
          LEAVE player_mare_art_loop;
        END IF;

    	-- SELECT playerMareId, playerMareSkill, playerMareSkillLevel;
        INSERT INTO skill(player_id, skill, skill_level) VALUES(playerMareId, playerMareSkill, playerMareSkillLevel);
        SELECT totalArtsInserted + ROW_COUNT() INTO totalArtsInserted;
      END LOOP player_mare_art_loop;

  END LOOP player_mare_loop;

DROP TEMPORARY TABLE temp_playermare_arts;

SELECT totalArtsInserted;
 
END//

