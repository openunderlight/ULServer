-- MySQL dump 10.13  Distrib 5.1.73, for redhat-linux-gnu (x86_64)
--
-- Host: localhost    Database: ul_server
-- ------------------------------------------------------
-- Server version	5.1.73

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Current Database: `ul_server`
--

/*!40000 DROP DATABASE IF EXISTS `ul_server`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `ul_server` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `ul_server`;

--
-- Table structure for table `debug_err`
--

DROP TABLE IF EXISTS `debug_err`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `debug_err` (
  `creation_time` date NOT NULL,
  `err_code` int(11) DEFAULT NULL,
  `err_text` varchar(200) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `debug_err`
--

LOCK TABLES `debug_err` WRITE;
/*!40000 ALTER TABLE `debug_err` DISABLE KEYS */;
/*!40000 ALTER TABLE `debug_err` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `server`
--

DROP TABLE IF EXISTS `server`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `server` (
  `host_name` varchar(32) NOT NULL,
  `host_id` varchar(10) NOT NULL,
  `server_type` char(1) NOT NULL,
  `arg1` int(10) unsigned NOT NULL,
  `arg2` int(10) unsigned NOT NULL,
  `allow_players` enum('YES','NO') DEFAULT 'YES'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `server`
--

LOCK TABLES `server` WRITE;
/*!40000 ALTER TABLE `server` DISABLE KEYS */;
INSERT INTO `server` VALUES ('199.58.75.232','1','G',7500,90,'YES'),('199.58.75.232','1','G',7501,90,'YES'),('199.58.75.232','1','G',7502,90,'YES'),('199.58.75.232','1','G',7503,90,'YES'),('199.58.75.232','1','G',7504,90,'YES'),('199.58.75.232','1','G',7505,90,'YES'),('199.58.75.232','1','G',7506,90,'NO'),('199.58.75.232','1','G',7507,90,'NO'),('199.58.75.232','1','G',7508,90,'NO'),('199.58.75.232','1','G',7509,90,'NO'),('199.58.75.232','1','L',7402,2,'YES'),('199.58.75.232','1','L',7403,3,'YES'),('199.58.75.232','1','L',7404,4,'YES'),('199.58.75.232','1','L',7405,5,'YES'),('199.58.75.232','1','L',7406,6,'YES'),('199.58.75.232','1','L',7407,7,'YES'),('199.58.75.232','1','L',7408,8,'YES'),('199.58.75.232','1','L',7409,9,'YES'),('199.58.75.232','1','L',7410,10,'YES'),('199.58.75.232','1','L',7411,11,'YES'),('199.58.75.232','1','L',7412,12,'YES'),('199.58.75.232','1','L',7413,13,'YES'),('199.58.75.232','1','L',7414,14,'YES'),('199.58.75.232','1','L',7415,15,'YES'),('199.58.75.232','1','L',7416,16,'YES'),('199.58.75.232','1','L',7417,17,'YES'),('199.58.75.232','1','L',7418,18,'YES'),('199.58.75.232','1','L',7419,19,'YES'),('199.58.75.232','1','L',7420,20,'YES'),('199.58.75.232','1','L',7421,21,'YES'),('199.58.75.232','1','L',7422,22,'YES'),('199.58.75.232','1','L',7423,23,'YES'),('199.58.75.232','1','L',7424,24,'YES'),('199.58.75.232','1','L',7425,25,'YES'),('199.58.75.232','1','L',7426,26,'YES'),('199.58.75.232','1','L',7427,27,'YES'),('199.58.75.232','1','L',7428,28,'YES'),('199.58.75.232','1','L',7429,29,'YES'),('199.58.75.232','1','L',7430,30,'YES'),('199.58.75.232','1','L',7431,31,'YES'),('199.58.75.232','1','L',7432,32,'YES'),('199.58.75.232','1','L',7433,33,'YES'),('199.58.75.232','1','L',7434,34,'YES'),('199.58.75.232','1','L',7435,35,'YES'),('199.58.75.232','1','L',7436,36,'YES'),('199.58.75.232','1','L',7437,37,'YES'),('199.58.75.232','1','L',7438,38,'YES'),('199.58.75.232','1','L',7439,39,'YES'),('199.58.75.232','1','L',7440,40,'YES'),('199.58.75.232','1','L',7441,41,'YES'),('199.58.75.232','1','L',7442,42,'YES'),('199.58.75.232','1','L',7443,43,'YES'),('199.58.75.232','1','L',7444,44,'YES'),('199.58.75.232','1','L',7445,45,'YES'),('199.58.75.232','1','L',7446,46,'YES'),('199.58.75.232','1','L',7447,47,'YES'),('199.58.75.232','1','L',7448,48,'YES'),('199.58.75.232','1','L',7449,49,'YES'),('199.58.75.232','1','L',7450,50,'YES'),('199.58.75.232','1','L',7451,51,'YES');
/*!40000 ALTER TABLE `server` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-10-06 21:00:25
