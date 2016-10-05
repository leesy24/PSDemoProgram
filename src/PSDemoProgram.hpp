/**
 ****************************************************************************
 *
 * Project:     Triple-IN PS Demo Program
 *
 * @file:       PSDemoProgram.hpp
 *
 * @author:     CBruelle
 *
 * Date:        $Date: 2015/10/23 12:23:10 $
 * SDK:         GNU gcc 4
 *
 * (c) 2014 Triple-IN GmbH Hamburg, Germany
 *
 * All rights reserved. Using, copying, publishing
 * or distributing  is not permitted without prior
 * written agreement of Triple-IN GmbH.
 ****************************************************************************
 * Last changes:
 * 24.04.2014 CBD:
 * - Changed: use IDataStream for communication.
 */

#ifndef PSDEMOPROGRAM_HPP_
#define PSDEMOPROGRAM_HPP_

/**
 * Main for a number of tests.
 *
 * @param argc
 *      number of command line arguments; used if ==3.
 * @param argv
 *      Pointer on the list of arguments:<br>
 *      Sensor IP address<br>
 *      Sensor IP port<br>
 *      Client IP address<br>
 *      Client port<br>
 *      Log file name<br>
 */
int
main(int argc, char **argv);

#endif /* PSDEMOPROGRAM_HPP_ */
