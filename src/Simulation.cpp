/*
 * a unit of
 * conjunction: simulator of secondary contact using Fisher's representation of genome admixture
 * handling simulation itself and providing API usable by any other wrapper
Copyright (C) 2016  Kamil S. Jaron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include <cmath>
#include <iomanip>

#include "../include/Chromosome.h"
#include "../include/Individual.h"
#include "../include/Imigrant.h"
#include "../include/SelectionModel.h"
#include "../include/Deme.h"
#include "../include/World.h"
#include "../include/SimulationSetting.h"
#include "../include/Simulation.h"

using namespace std;

Simulation::Simulation(SimulationSetting& simulation_setting) {
	World world;

	setWorld(simulation_setting);
}

int Simulation::simulate(){

	if(saves > 10){
		file_name = file_name + "_0*.tsv";
	} else {
		if(saves > 1 and saves < 10){
			file_name = file_name + "_*.tsv";
		} else {
			file_name = file_name + ".tsv";
		}
	}
	int save_pos = file_name.find('*');

	if((saves > 0) & (file_name[0] != '_')){
		cerr << "*** OUTPUT INFO ***" << endl;
		cerr << "Name of the file: " << file_name << endl;
		cerr << "Type of the file: " << file_type << endl;
		cerr << "Number of files: " << saves << endl;
		cerr << "*******************" << endl;
	}

	clock_t t_total1, t_total2;
	t_total1 = clock();
	clock_t t1, t2;

	int order = 0, check = 0, modulo = ceil((double)(generations-delay-1) / saves);
	// modulo only for non-zero saves
	// cout << modulo << endl;
	world.restart();

	for(int i=0; i < generations;i++){
		t1=clock();
		world.migration();
		world.globalBreeding();
		t2=clock();
		cerr << "Generation: " << i + 1 << " done in " << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << endl;
		if((((i - delay) % modulo)+1) == modulo and (i < generations - modulo or i+1 == generations)){
			world.summary();
			order++;
			if(saves > 1){ // add non-zero lengyth of file_name
				if(order >= 10){
					file_name[save_pos-1] = '0' + char(order / 10 % 10);
					file_name[save_pos] = '0' + char(order % 10);
				} else {
					file_name[save_pos] = '0' + char(order);
				}
				cerr << "Saving output to: " << file_name << endl;
				check = world.SaveTheUniverse(file_type, file_name);
				if(check != 0){
					cerr << "Error in saving the output." << endl;
					return 1;
				}
			}
		}
	}

	world.summary();

	t_total2 = clock();
	cerr << "Total time of simulation: " << ((float)t_total2 - (float)t_total1) / CLOCKS_PER_SEC << endl;
	world.clear();

	return 0;
}

void Simulation::setWorld(SimulationSetting& simulation_setting){

	saves = simulation_setting.saves;
	delay = simulation_setting.delay;
	generations = simulation_setting.generations;
	file_name = simulation_setting.file_to_save;
	file_type = simulation_setting.type_of_save;

	srand (simulation_setting.seed);

	world.setSlectionBetaLambda(simulation_setting.selection, simulation_setting.beta, simulation_setting.lambda);
	world.setLociChromDeme(simulation_setting.loci, simulation_setting.chromosomes, simulation_setting.deme_size);

	world.setHeight(simulation_setting.up_down_demes);
	world.setWidth(simulation_setting.left_right_demes);
	world.setLREdgesType(simulation_setting.type_of_leftright_edges);
	world.setUDEdgesType(simulation_setting.type_of_updown_edges);
	world.setDimension(simulation_setting.dimension);
	world.setNumberOfEdges(simulation_setting.edges_per_deme);

	return;
}
