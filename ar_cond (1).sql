-- phpMyAdmin SQL Dump
-- version 4.8.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: 07-Nov-2018 às 21:14
-- Versão do servidor: 10.1.31-MariaDB
-- PHP Version: 5.6.35

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `ar_cond`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `ar_cond`
--

CREATE TABLE `ar_cond` (
  `id` int(6) UNSIGNED NOT NULL,
  `sala` varchar(25) NOT NULL,
  `endereco` varchar(15) NOT NULL,
  `temp` float NOT NULL,
  `humi` float NOT NULL,
  `corrente` float NOT NULL,
  `pot` float NOT NULL,
  `khw` float NOT NULL,
  `v_reais` float NOT NULL,
  `data` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `ar_cond`
--

INSERT INTO `ar_cond` (`id`, `sala`, `endereco`, `temp`, `humi`, `corrente`, `pot`, `khw`, `v_reais`, `data`) VALUES
(1, 'Lab 117', '0x01', 0, 0, 0.51, 64, 0.01878, 0.00657, '2018-11-07 13:58:22');

-- --------------------------------------------------------

--
-- Estrutura da tabela `teste`
--

CREATE TABLE `teste` (
  `id` int(6) UNSIGNED NOT NULL,
  `sala` varchar(25) NOT NULL,
  `endereco` varchar(15) NOT NULL,
  `temp` float NOT NULL,
  `humi` float NOT NULL,
  `corrente` float NOT NULL,
  `pot` float NOT NULL,
  `khw` float NOT NULL,
  `v_reais` float NOT NULL,
  `data` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `teste`
--

INSERT INTO `teste` (`id`, `sala`, `endereco`, `temp`, `humi`, `corrente`, `pot`, `khw`, `v_reais`, `data`) VALUES
(1, 'Lab 117', '0', 0, 0, 0.52, 65, 0.00643, 0.00225, '2018-11-07 13:26:18');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `ar_cond`
--
ALTER TABLE `ar_cond`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `teste`
--
ALTER TABLE `teste`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `ar_cond`
--
ALTER TABLE `ar_cond`
  MODIFY `id` int(6) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
