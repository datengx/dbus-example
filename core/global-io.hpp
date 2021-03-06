/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014  Regents of the University of California,
 *                     Arizona Board of Regents,
 *                     Colorado State University,
 *                     University Pierre & Marie Curie, Sorbonne University,
 *                     Washington University in St. Louis,
 *                     Beijing Institute of Technology
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef NFD_CORE_GLOBAL_IO_HPP
#define NFD_CORE_GLOBAL_IO_HPP

#include "common.hpp"

namespace nfd {

/** \return the global io_service instance
 */
boost::asio::io_service&
getGlobalIoService();

#ifdef WITH_TESTS
/** \brief delete the global io_service instance
 *
 *  It will be recreated at the next invocation of getGlobalIoService.
 */
void
resetGlobalIoService();
#endif

} // namespace nfd

#endif // NFD_CORE_GLOBAL_IO_HPP
