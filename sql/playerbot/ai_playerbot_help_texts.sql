-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               5.7.26 - MySQL Community Server (GPL)
-- Server OS:                    Win32
-- HeidiSQL Version:             12.2.0.6576
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

-- Dumping structure for table classicplayerbots.ai_playerbot_help_texts
DROP TABLE IF EXISTS `ai_playerbot_help_texts`;
CREATE TABLE IF NOT EXISTS `ai_playerbot_help_texts` (
  `id` smallint(20) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL COMMENT 'name - type:subject',
  `template_changed` tinyint(4) NOT NULL DEFAULT '0' COMMENT 'template_changed - Has the template text changed after text?',
  `template_text` text NOT NULL COMMENT 'generated text',
  `text` text NOT NULL COMMENT 'text',
  `text_loc1` text NOT NULL,
  `text_loc2` text NOT NULL,
  `text_loc3` text NOT NULL,
  `text_loc4` text NOT NULL,
  `text_loc5` text NOT NULL,
  `text_loc6` text NOT NULL,
  `text_loc7` text NOT NULL,
  `text_loc8` text NOT NULL,
  `locs_updated` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'locs_updated - Have the loc texts been updated?',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- Dumping data for table classicplayerbots.ai_playerbot_help_texts: ~10 rows (approximately)
INSERT INTO `ai_playerbot_help_texts` (`id`, `name`, `template_changed`, `template_text`, `text`, `text_loc1`, `text_loc2`, `text_loc3`, `text_loc4`, `text_loc5`, `text_loc6`, `text_loc7`, `text_loc8`, `locs_updated`) VALUES
	(1, 'help:main', 0, '----This is the main help page----\r\nPlease copy a link to get more information about a topic.\r\nObjects: [h:object:strategy|strategies][h:object:trigger|triggers][h:object:action|actions][h:object:value|values][h:object:chatfilter|chatfilters]', '', '', '', '', '', '', '', '', '', 0),
	(2, 'object:strategy', 0, 'Strategies are rules that controle bot behavior. They combine [h:object:trigger|triggers] and [h:object:action|actions].\r\nStrategies: [h:list:generic strategy|general]\r\n[h:list:warrior strategy|warrior][h:list:paladin strategy|paladin][h:list:hunter strategy|hunter]\r\n[h:list:rogue strategy|rogue][h:list:priest strategy|priest][h:list:deathknight strategy|deathknight]\r\n[h:list:shaman strategy|shaman][h:list:mage strategy|mage][h:list:warlock strategy|warlock][h:list:druid strategy|druid]', '', '', '', '', '', '', '', '', '', 0),
	(3, 'object:trigger', 0, 'Triggers are conditions are evalated by the bot which helps it decide what to do. \r\nA [h:object|strategy] will enable a trigger and combine it with one or multiple [h:object:action|actions] which results in bot behavior. \r\nTriggers: [h:list:generic trigger|general]\r\n[h:list:warrior trigger|warrior][h:list:paladin trigger|paladin][h:list:hunter trigger|hunter]\r\n[h:list:rogue trigger|rogue][h:list:priest trigger|priest][h:list:deathknight trigger|deathknight]\r\n[h:list:shaman trigger|shaman][h:list:mage trigger|mage][h:list:warlock trigger|warlock][h:list:druid trigger|druid]', '', '', '', '', '', '', '', '', '', 0),
	(4, 'object:action', 0, 'Actions are specific interactions the bot can have with the world or itself. \r\nUsually the result of a [h:object|trigger] enabled by a [h:object|strategy].\r\nTriggers: [h:list:generic action|general]\r\n[h:list:warrior action|warrior][h:list:paladin action|paladin][h:list:hunter action|hunter]\r\n[h:list:rogue action|rogue][h:list:priest action|priest][h:list:deathknight action|deathknight]\r\n[h:list:shaman action|shaman][h:list:mage action|mage][h:list:warlock action|warlock][h:list:druid action|druid]', '', '', '', '', '', '', '', '', '', 0),
	(5, 'object:value', 0, 'Values are pieces of information the bot has about itself or the world.\r\nThey are used in [h:object:trigger|triggers] and [h:object:action|actions] for evaluation purposes.\r\nValues:', '', '', '', '', '', '', '', '', '', 0),
	(6, 'template:strategy', 0, '<name> [h:object|strategy] [c:co ~<name>|toggle combat][c:nc ~<name>|toggle noncombat]\r\n<description><related><behavior>', '', '', '', '', '', '', '', '', '', 0),
	(7, 'template:trigger', 0, '<name> [h:object|trigger] [c:<name>|trigger now]\r\n<description><used trig><used val><behavior>', '', '', '', '', '', '', '', '', '', 0),
	(8, 'template:action', 0, '<name> [h:object|action] [c:do <name>|execute]\r\n<description><used act><used val><behavior>', '', '', '', '', '', '', '', '', '', 0),
	(9, 'template:value', 0, '<name> [h:object|value] [c:cdebug values <name>|current value]\r\n<description><used val><used in trig><used in act><used in val>', '', '', '', '', '', '', '', '', '', 0),
	(10, 'object:chatfilter', 0, 'Chatfilters is a bot selection mechanism that can be precede a chat command to only reach specific bots.\r\nExample "@<filter> who" will only make bots who fit the filter respond.\r\nMultiple filters can be used at the same time.\r\nFilters:', '', '', '', '', '', '', '', '', '', 0),
	(11, 'template:chatfilter', 0, '<name> [h:object|chatfilter]\r\n<description>\r\nExamples:\r\n<examples>', '', '', '', '', '', '', '', '', '', 0);

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
