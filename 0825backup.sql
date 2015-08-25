-- MySQL dump 10.13  Distrib 5.1.73, for redhat-linux-gnu (i386)
--
-- Host: localhost    Database: gprs
-- ------------------------------------------------------
-- Server version	5.1.73-log

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
-- Table structure for table `RealTimeData`
--

DROP TABLE IF EXISTS `RealTimeData`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `RealTimeData` (
  `id` int(11) DEFAULT NULL,
  `current` int(11) DEFAULT NULL,
  `voltage` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `RealTimeData`
--

LOCK TABLES `RealTimeData` WRITE;
/*!40000 ALTER TABLE `RealTimeData` DISABLE KEYS */;
/*!40000 ALTER TABLE `RealTimeData` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `VoltageMonitor`
--

DROP TABLE IF EXISTS `VoltageMonitor`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `VoltageMonitor` (
  `id` int(11) NOT NULL,
  `current` int(11) NOT NULL,
  `voltage` int(11) NOT NULL,
  `d` date DEFAULT NULL,
  `t` time DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `VoltageMonitor`
--

LOCK TABLES `VoltageMonitor` WRITE;
/*!40000 ALTER TABLE `VoltageMonitor` DISABLE KEYS */;
INSERT INTO `VoltageMonitor` VALUES (1,0,221,'2015-08-18','00:00:00'),(1,0,221,'2015-08-18','01:00:00'),(1,0,221,'2015-08-18','02:00:00'),(1,0,221,'2015-08-18','03:00:00'),(1,0,221,'2015-08-18','04:00:00'),(1,0,221,'2015-08-18','05:00:00'),(1,0,221,'2015-08-18','06:00:00'),(1,0,221,'2015-08-18','07:00:00'),(1,0,221,'2015-08-18','08:00:00'),(1,0,221,'2015-08-18','09:00:00'),(1,0,221,'2015-08-18','20:00:00'),(1,0,221,'2015-08-18','11:00:00'),(1,0,221,'2015-08-18','12:00:00'),(1,0,221,'2015-08-18','13:00:00'),(1,0,221,'2015-08-18','14:00:00'),(1,0,221,'2015-08-18','15:00:00'),(1,0,221,'2015-08-18','16:00:00'),(1,0,221,'2015-08-18','17:00:00'),(1,0,221,'2015-08-18','18:00:00'),(1,0,221,'2015-08-18','19:00:00'),(1,0,221,'2015-08-18','10:00:00'),(1,0,221,'2015-08-18','21:00:00'),(1,0,221,'2015-08-18','22:00:00'),(1,0,221,'2015-08-18','23:00:00');
/*!40000 ALTER TABLE `VoltageMonitor` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `demo`
--

DROP TABLE IF EXISTS `demo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `demo` (
  `id` int(11) DEFAULT NULL,
  `current` int(11) DEFAULT NULL,
  `voltage` int(11) DEFAULT NULL,
  `d` date DEFAULT NULL,
  `t` time DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `demo`
--

LOCK TABLES `demo` WRITE;
/*!40000 ALTER TABLE `demo` DISABLE KEYS */;
INSERT INTO `demo` VALUES (1,30,223,'2015-07-06','10:59:08'),(1,30,223,'2015-07-06','10:59:08'),(1,30,223,'2015-07-06','10:58:54'),(1,30,223,'2015-07-06','10:58:54'),(4,47,0,'2015-07-07','17:53:07'),(1,30,223,'2015-07-06','10:58:52'),(1,30,223,'2015-07-06','10:58:41'),(1,30,223,'2015-07-06','09:35:59'),(1,30,223,'2015-07-04','22:48:32'),(4,45,0,'2015-07-07','17:53:04'),(4,43,0,'2015-07-07','13:43:32'),(4,46,0,'2015-07-07','11:53:47'),(4,42,0,'2015-07-07','11:53:35'),(4,44,0,'2015-07-07','08:53:59'),(4,44,0,'2015-07-06','14:52:54'),(4,36,0,'2015-07-06','14:52:06'),(4,43,0,'2015-07-06','14:45:59'),(4,37,0,'2015-07-06','14:42:34'),(4,52,0,'2015-07-08','09:12:44'),(4,51,0,'2015-07-08','09:13:04'),(4,52,0,'2015-07-08','09:13:45'),(4,51,0,'2015-07-08','09:13:52'),(4,50,0,'2015-07-08','11:52:09'),(4,53,0,'2015-07-08','13:58:20'),(4,44,0,'2015-07-08','20:48:34'),(4,52,0,'2015-07-08','20:48:37'),(4,46,0,'2015-07-09','11:25:36'),(4,150,0,'2015-07-09','11:25:58'),(4,26,0,'2015-07-09','11:26:07'),(4,37,0,'2015-07-09','11:52:26'),(4,35,0,'2015-07-09','11:56:15'),(3,0,0,'2015-07-09','14:15:58'),(3,0,0,'2015-07-09','14:17:28'),(3,0,0,'2015-07-09','14:20:30'),(3,0,0,'2015-07-09','14:20:30'),(3,136,0,'2015-07-09','14:47:35'),(3,122,0,'2015-07-09','14:50:21'),(3,165,0,'2015-07-09','14:51:12'),(4,146,231,'2015-07-09','14:56:26'),(4,162,238,'2015-07-09','14:58:15'),(2,170,237,'2015-07-09','15:10:02'),(3,80,0,'2015-07-09','15:19:21'),(5,30,223,'2015-07-13','21:22:41'),(5,30,223,'2015-07-13','21:22:49'),(5,30,223,'2015-07-13','21:23:00'),(5,30,223,'2015-07-13','21:23:09'),(5,30,223,'2015-07-13','21:23:33'),(4,29,0,'2015-07-16','13:06:55'),(4,21,0,'2015-07-16','13:06:57'),(5,30,223,'2015-07-24','21:30:51'),(5,30,223,'2015-07-24','21:31:01'),(5,30,223,'2015-07-24','21:31:11'),(5,30,223,'2015-07-24','21:31:21'),(5,30,223,'2015-07-24','21:31:31'),(5,30,223,'2015-07-24','21:31:41'),(5,30,223,'2015-07-24','21:31:51');
/*!40000 ALTER TABLE `demo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-08-25 16:25:09
