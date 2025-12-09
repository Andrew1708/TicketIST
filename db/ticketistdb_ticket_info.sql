-- MySQL dump 10.13  Distrib 8.0.36, for macos14 (x86_64)
--
-- Host: 127.0.0.1    Database: ticketistdb
-- ------------------------------------------------------
-- Server version	8.3.0

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `ticket_info`
--

DROP TABLE IF EXISTS `ticket_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `ticket_info` (
  `ticketID` int NOT NULL AUTO_INCREMENT,
  `eventID` int NOT NULL,
  `sellerID` int NOT NULL,
  `price` double NOT NULL,
  `seat` varchar(10) NOT NULL,
  PRIMARY KEY (`ticketID`)
) ENGINE=InnoDB AUTO_INCREMENT=81 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ticket_info`
--

LOCK TABLES `ticket_info` WRITE;
/*!40000 ALTER TABLE `ticket_info` DISABLE KEYS */;
INSERT INTO `ticket_info` VALUES (1,1,0,40,'86L'),(3,1,0,70,'92B'),(4,1,0,20,'62D'),(5,1,0,10,'59Z'),(6,1,0,70,'40K'),(7,1,0,30,'36H'),(8,1,0,90,'67D'),(9,1,0,30,'30C'),(10,1,0,40,'67R'),(11,1,0,100,'2O'),(12,1,0,90,'69R'),(13,1,0,40,'56J'),(14,1,0,30,'29L'),(15,1,0,20,'19E'),(16,1,0,80,'98A'),(17,1,0,60,'70B'),(18,1,0,70,'91E'),(19,1,0,70,'73Y'),(20,1,0,10,'96X'),(21,2,0,60,'25K'),(22,2,0,80,'36R'),(23,2,0,70,'29L'),(24,2,0,80,'24P'),(25,2,0,30,'45Q'),(26,2,0,80,'34K'),(27,2,0,40,'50P'),(28,2,0,90,'76C'),(29,2,0,90,'84H'),(30,2,0,20,'54N'),(31,2,0,30,'60E'),(32,2,0,90,'39S'),(33,2,0,70,'86G'),(34,2,0,100,'95U'),(35,2,0,50,'78L'),(36,2,0,20,'97I'),(37,2,0,80,'92W'),(38,2,0,70,'1K'),(39,2,0,70,'41B'),(40,2,0,100,'44X'),(41,3,0,10,'29T'),(42,3,0,80,'97B'),(43,3,0,20,'75P'),(44,3,0,80,'67S'),(45,3,0,80,'53W'),(46,3,0,60,'6F'),(47,3,0,100,'24A'),(48,3,0,20,'32D'),(49,3,0,40,'19W'),(50,3,0,90,'8F'),(51,3,0,10,'49C'),(52,3,0,40,'18V'),(53,3,0,70,'51F'),(54,3,0,60,'79S'),(55,3,0,50,'28X'),(56,3,0,10,'93Q'),(57,3,0,50,'64C'),(58,3,0,50,'87C'),(59,3,0,40,'91V'),(60,3,0,60,'59K'),(61,4,0,30,'51N'),(62,4,0,90,'75J'),(63,4,0,50,'21Q'),(64,4,0,60,'29R'),(65,4,0,60,'93K'),(66,4,0,90,'43Z'),(67,4,0,90,'29A'),(68,4,0,50,'43H'),(69,4,0,40,'38I'),(70,4,0,10,'4C'),(71,4,0,90,'88N'),(72,4,0,80,'17O'),(73,4,0,50,'43G'),(74,4,0,40,'90D'),(75,4,0,30,'25C'),(76,4,0,10,'5X'),(77,4,0,50,'86V'),(78,4,0,30,'42Y'),(79,4,0,80,'7L'),(80,4,0,50,'48D');
/*!40000 ALTER TABLE `ticket_info` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-12-30 22:51:39
