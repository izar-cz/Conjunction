/*
World, (Universe previously) was a wrapping class for set of demes with given interactions.
Copyright (C) 2013-2016  Kamil S. Jaron

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
#include <string>
#include <math.h>
#include <cmath>
#include <iomanip>
#include <unistd.h>

#include "../include/RandomGenerators.h"
#include "../include/Imigrant.h"
#include "../include/SelectionModel.h"
#include "../include/Deme.h"
#include "../include/World.h"

World::World(const SimulationSetting& simulationSetting) :
	context{simulationSetting.seed},
	// USER
	// space
	number_of_demes_l_r{simulationSetting.left_right_demes},
	number_of_demes_u_d{simulationSetting.up_down_demes},
	dimension{simulationSetting.dimension},
	type_of_l_r_edges{simulationSetting.type_of_leftright_edges},
	type_of_u_d_edges{simulationSetting.type_of_updown_edges},

	deme_size{simulationSetting.deme_size},
	number_of_chromosomes{simulationSetting.chromosomes},
	number_of_loci{simulationSetting.loci},
	number_of_selected_loci{
		-1 == simulationSetting.selected_loci
			? simulationSetting.loci
			: simulationSetting.selected_loci
	},

	selection{simulationSetting.selection},
	beta{simulationSetting.beta},
	lambda{simulationSetting.lambda},
	// INTERNAL
	edges_per_deme{simulationSetting.edges_per_deme},
	index_last_left{-1},
	index_next_left{-1},
	index_last_right{-1},
	index_next_right{-1}
{}

void World::basicUnitCreator(char type, char init){
	int max_index = world.size();
	int x_coordinate = 0;
	std::vector<int> new_indexes;
	int index;
// 	1D world definition
	if(dimension == 1){
		switch (type) {
			case 'b':
				index_next_left = 1;
				index_next_right = 2;
				index_last_left = 0;
				index_last_right = 0;
				new_indexes.clear();
				new_indexes.push_back(sideBorder(0,index_next_left));
				if(number_of_demes_l_r == 1){
					new_indexes.push_back(sideBorder(0,index_next_right));
				} else {
					new_indexes.push_back(index_next_right);
				}
				world[0] = new Deme(context, 0,new_indexes,init, deme_size, selection, beta, number_of_chromosomes, number_of_loci, number_of_selected_loci, lambda, x_coordinate, 0);
				break;
			case 'l':
				new_indexes.clear();
				new_indexes.push_back(max_index + 2);
				new_indexes.push_back(index_last_left);
				x_coordinate = world[index_last_left]->getX() - 1;
				world[index_next_left] = new Deme(context, index_next_left,new_indexes,init, deme_size, selection, beta, number_of_chromosomes, number_of_loci, number_of_selected_loci, lambda, x_coordinate, 0);
				index_last_left = index_next_left;
				index_next_left = max_index + 2;
				break;
			case 'r':
				new_indexes.clear();
				new_indexes.push_back(index_last_right);
				if(type_of_l_r_edges == "wrapping"){
					if(index_next_right == number_of_demes_l_r){
						new_indexes.push_back(0);
					} else {
						new_indexes.push_back(max_index + 2);
					}
				}
				if(type_of_l_r_edges == "reflexive"){
					if(index_next_right == number_of_demes_l_r){
						new_indexes.push_back(index_next_right);
					} else {
						new_indexes.push_back(max_index + 2);
					}
				}
				if(type_of_l_r_edges == "extending"){
					new_indexes.push_back(max_index + 2);
				}
				if(type_of_l_r_edges == "infinite"){
					if(index_next_right == number_of_demes_l_r){
						new_indexes.push_back(-8);
					} else {
						new_indexes.push_back(max_index + 2);
					}
				}
				x_coordinate = world[index_last_right]->getX() + 1;
				world[index_next_right] = new Deme(context, index_next_right,new_indexes,init, deme_size, selection, beta, number_of_chromosomes, number_of_loci, number_of_selected_loci, lambda, x_coordinate, 0);
				index_last_right = index_next_right;
				index_next_right = max_index + 2;
				break;

		}
		return;
	}

// 	2D world definition
	switch (type) {
		case 'b':
			index_next_left = number_of_demes_u_d;
			index_next_right = number_of_demes_u_d * 2;
			index_last_left = 0;
			index_last_right = 0;
			for(int i=0;i<number_of_demes_u_d;i++){
				new_indexes.clear();
				new_indexes.push_back(sideBorder(i,index_next_left + i));
				new_indexes.push_back(i + number_of_demes_u_d * 2);
				new_indexes.push_back(upperBorder(i,max_index));
				new_indexes.push_back(lowerBorder(i,max_index));
				world[i] = new Deme(context, i,new_indexes,init, deme_size, selection, beta, number_of_chromosomes, number_of_loci, number_of_selected_loci, lambda, 0, i);
			}
			break;
		case 'l':
			index = index_next_left;
			index_next_left = max_index + 2 * number_of_demes_u_d;
			x_coordinate = world[index_last_left]->getX() - 1;
			for(int i=0;i<number_of_demes_u_d;i++){
				new_indexes.clear();
				new_indexes.push_back(sideBorder(index + i,index_next_left + i));
				new_indexes.push_back(index_last_left + i);
				new_indexes.push_back(upperBorder(index + i,index));
				new_indexes.push_back(lowerBorder(index + i,index));
				world[index + i] = new Deme(context, index + i,new_indexes,init, deme_size, selection, beta, number_of_chromosomes, number_of_loci, number_of_selected_loci, lambda, x_coordinate, i);
			}
			index_last_left = index;
			break;
		case 'r':
			index = index_next_right;
			index_next_right = max_index + 2 * number_of_demes_u_d;
			x_coordinate = world[index_last_right]->getX() + 1;
			for(int i=0;i<number_of_demes_u_d;i++){
				new_indexes.clear();
				new_indexes.push_back(index_last_right + i);
				new_indexes.push_back(sideBorder(index + i,index_next_right + i));
				new_indexes.push_back(upperBorder(index + i,index));
				new_indexes.push_back(lowerBorder(index + i,index));
				world[index + i] = new Deme(context, index + i,new_indexes,init, deme_size, selection, beta, number_of_chromosomes, number_of_loci, number_of_selected_loci, lambda, x_coordinate, i);
			}
			index_last_right = index;
			break;
		default:
			std::cerr << "Error: I have no idea, what do you mean by... " << type << " I understand only 'b' basic, 'l' left and 'r' right, type of basic unit.. try it again please." << std::endl;
			break;
	}
	return;
}

void World::worldSlave(){
	if(number_of_demes_l_r == 1){
		basicUnitCreator('b', 'C');
		return;
	}
	if(number_of_demes_l_r == 2){
		basicUnitCreator('b', 'A');
		basicUnitCreator('r', 'B');
		return;
	}

	double midpoint = double(number_of_demes_l_r + 1) / 2;
	basicUnitCreator('b', 'A');

	for(double i = 2; i < number_of_demes_l_r; i++){
		if(i < midpoint){
			basicUnitCreator('r', 'A');
		} else {
			basicUnitCreator('r', 'B');
		}
	}
	basicUnitCreator('r', 'B');

	return;
}

  // // // // // // // //
 // COMPUTING METHODS //
// // // // // // // //

int World::migration(){
//	const int demesize = deme_size;
	if(dimension == 0){
//		std::cerr << "Premigration Population size: " << zeroD_immigrant_pool.size() << std::endl;
		zeroD_immigrant_pool.reserve(zeroD_immigrant_pool.size() + deme_size);
		for(int i = 0; i < deme_size;i++){
			zeroD_immigrant_pool.push_back(Imigrant(context, number_of_chromosomes, number_of_loci, lambda));
		}
//		std::cerr << "Postmigration Population size: " << zeroD_immigrant_pool.size() << std::endl;
		return 0;
	}
	if(world.empty()){
		std::cerr << "ERROR: Missing demes" << std::endl;
		return -1;
	}
	if(edges_per_deme == 0){
		return 0;
	}

	int index_last_left_fix = index_last_left;
	int index_last_right_fix = index_last_right;
	std::map<int, std::vector<Individual> > ImmigranBuffer;

	std::vector<int> neigbours;
	int MigInd = deme_size / (2 * edges_per_deme );
	int deme_index;
	int cartesian_x = 0, cartesian_y = 0;
	/*bufferVectorMap is container for all individuals imigrating to all demes*/
	for (std::map<int, Deme*>::const_iterator deme=world.begin(); deme!=world.end(); ++deme){
		neigbours = deme->second->getNeigbours();
		for(unsigned int j=0;j < neigbours.size();j++){
			deme_index = neigbours[j];
			if(deme_index == -8){
				continue;
			}
			for(int k=0;k < MigInd; k++){
				ImmigranBuffer[deme_index].push_back(deme->second->getIndividual(k));
			}
		}
	}

	for(std::map<int, std::vector<Individual> >::iterator buff=ImmigranBuffer.begin(); buff!=ImmigranBuffer.end(); ++buff){
		cartesian_y = buff->first % number_of_demes_u_d;
		if(buff->first >= index_last_left_fix and buff->first < index_last_left_fix + number_of_demes_u_d){
			cartesian_x = world[index_last_left_fix]->getX() - 1;
			for(int k=0;k < MigInd; k++){
				ImmigranBuffer[buff->first].push_back(Individual(&context, 'A', number_of_chromosomes, number_of_loci, lambda, number_of_selected_loci, std::tuple<int, int, int>(cartesian_x, cartesian_y, k)));
			}
		}
		if(buff->first >= index_last_right_fix and buff->first < index_last_right_fix + number_of_demes_u_d){
			cartesian_x = world[index_last_right_fix]->getX() + 1;
			for(int k=0;k < MigInd; k++){
				ImmigranBuffer[buff->first].push_back(Individual(&context, 'B', number_of_chromosomes, number_of_loci, lambda, number_of_selected_loci, std::tuple<int, int, int>(cartesian_x, cartesian_y,  k)));
			}
		}
		if(index_next_left <= buff->first and buff->first < index_next_left + number_of_demes_u_d){
			if(isPureA(buff->second)){
				continue;
			}
			basicUnitCreator('l', 'A');
		}
		if(index_next_right <= buff->first and buff->first < index_next_right + number_of_demes_u_d){
			if(isPureB(buff->second)){
				continue;
			}
			basicUnitCreator('r', 'B');
		}
		world[buff->first]->integrateMigrantVector(buff->second);
	}
	return 0;
}

void World::globalBreeding(){
	if(dimension == 0){
	SelectionModel selection_model;
	selection_model.setSelectionPressure(selection);
	selection_model.setBeta(beta);

	double material = 0;
	for(unsigned int i = 0;i < zeroD_immigrant_pool.size();i++){
		material += zeroD_immigrant_pool[i].getBprop();
	}
		// std::cout << "Starting population size: " << zeroD_immigrant_pool.size() << std::endl;
		// std::cout << "Amount of material: " << material << std::endl;

		std::vector<Imigrant> new_generation;
		std::vector<Chromosome> gamete;

		new_generation.reserve(zeroD_immigrant_pool.size());
		gamete.reserve(number_of_chromosomes);
		double fitness = 0, hybrid_index = 0;
		int num_of_desc = 0;

		for(unsigned int index = 0; index < zeroD_immigrant_pool.size(); index++){
			hybrid_index = zeroD_immigrant_pool[index].getBprop() / 2;
			fitness = selection_model.getFitness(hybrid_index);
//			every7 individual has 2 attempts to mate
			for(int attempt = 0; attempt < 2; attempt++){
				num_of_desc = context.random.poisson(fitness);
				for(int i=0;i<num_of_desc;i++){
					zeroD_immigrant_pool[index].makeGamete(gamete);
					if(isGameteA(gamete)){
						continue;
					}
					new_generation.push_back( Imigrant(context, gamete, lambda) );
				}
			}
		}
		zeroD_immigrant_pool.clear(); // 1, this is incredibly stupid what I am doing here
		zeroD_immigrant_pool.swap(new_generation); // 2, I should change pointers instead of copy-pasting
		int pop_size = int(zeroD_immigrant_pool.size());
		for(int i = 0;i < pop_size;i++){
			material += zeroD_immigrant_pool[i].getBprop();
		}
		new_generation.clear(); // 3, in memory
		return;
	}

	std::vector<int> indexes;
	for (std::map<int, Deme*>::const_iterator i=world.begin(); i!=world.end(); ++i){
		indexes.push_back(i->first);
	}

	int index = 0;
	int i_size = indexes.size();

// 	#pragma omp parallel
// 	{
//
// 		{
// 		#ifdef _OPENMP
// 		int tnum = omp_get_num_threads();
// 		#else
// 		int tnum = 1;
// 		#endif
//
// 		std::cout << "Number of threads " << tnum << " "<< i_size << std::endl;
// 		}
//
// 	#pragma omp parallel for
		for(int i = 0; i < i_size; i++){
			index = indexes[i];
			world[index]->Breed();
		}
// 	}
	return;
}


  // // // // // // //
 // LOGICAL METHODS//
// // // // // // //

bool World::isPureA(std::vector<Individual>& buffer){
	for(int i = 0; (unsigned)i < buffer.size(); i++){
		if(buffer[i].isPureA()){
			continue;
		}
		return 0;
	}
	return 1;
}

bool World::isPureB(std::vector<Individual>& buffer){
	for(int i = 0; (unsigned)i < buffer.size(); i++){
		if(buffer[i].isPureB()){
			continue;
		}
		return 0;
	}
	return 1;
}

bool World::isEmpty(){
	return (world.size() == 0);
}

  // // // // // // // //
 // PLOTTING // STATS //
// // // // // // // //

void World::listOfParameters() const{
	std::cerr << "***************" << std::endl;
	if(dimension != 0){
		std::cerr << "Size of World: " << world.size() << " Dim: " << dimension << " edges_per_deme: " << edges_per_deme << std::endl
		<< "Number of demes l/r: " << number_of_demes_l_r << " Number of demes u/d: " << number_of_demes_u_d << std::endl
		<< "Type of l/r edges: " << type_of_l_r_edges << " Type of u/d edges: " << type_of_u_d_edges << std::endl
		<< "Last left index: " << index_last_left << " Last right index: " << index_last_right << std::endl
		<< "Next left index: " << index_next_left << " Next right index: " << index_next_right << std::endl
		<< "***************" << std::endl;
	}
	listOfNumericalParameters(std::cerr);
	std::cerr << "***************" << std::endl;
	return;
}

void World::listOfNumericalParameters(std::ostream& stream) const{
	stream << "# Selection: " << selection << std::endl
	<< "# Lambda: " << lambda << std::endl
	<< "# Beta: " << beta << std::endl
	<< "# Loci: " << number_of_loci << std::endl;
	if(number_of_selected_loci != number_of_loci){
		stream << "# Selected loci : " << number_of_selected_loci << std::endl;
	}
	stream << "# Chromosomes: " << number_of_chromosomes << std::endl;
	if(dimension == 0){
		stream << "# Migrants per generation: ";
	} else {
		stream << "# Deme size: ";
	}
	stream << deme_size << std::endl;
	return;
}

void World::listOfDemes(){
	std::cerr << "of dimension: " << dimension << std::endl;
	if(dimension == 0){
		std::cerr << "Population of imigrants has " << zeroD_immigrant_pool.size() << std::endl;
	} else {
		std::cerr << "World of size " << world.size() << std::endl;
		std::cerr << "Number of demes up to down: " << number_of_demes_u_d << std::endl;
		std::cerr << "Type of borders top and bottom: " << type_of_u_d_edges << std::endl;
		if(type_of_l_r_edges != "extending"){
			std::cerr << "Number of demes left to right: " << number_of_demes_l_r << std::endl;
		}
		std::cerr << "Type of borders left to right: " << type_of_l_r_edges << std::endl;
		std::cerr << "                 EDGE" << std::endl;
		std::cerr
			<< std::setw(7) << std::right << "DEME "
			<< std::setw(7) << std::left << " LEFT"
			<< std::setw(6) << std::left << "RIGHT"
			<< std::setw(5) << std::left << "UP"
			<< std::setw(6) << std::left << "DOWN"
			<< std::endl;

		int deme_to_print = index_last_left;
		while(deme_to_print != index_next_right){
			world[deme_to_print]->showDeme();
			deme_to_print = world[deme_to_print]->getNeigbours()[1];
		}

	}
}

int World::summary(std::ostream& stream){
	if(dimension == 0){
		stream << " 0D summary\n";
		stream << std::setw(12) << std::left <<  "Population"
		<< std::setw(12) << std::left << "Material"
		<< std::setw(16) << std::left << "TotalBlocks"
		<< std::setw(12) << std::left << "MeanFitness" << std::endl;
		stream << std::setw(12) << std::left <<  zeroD_immigrant_pool.size()
		<< std::setw(12) << std::left <<  getMaterial()
		<< std::setw(16) << std::left <<  getTotalNumberOfBBlocks()
		<< std::setw(12) << std::left <<  getMeanFitness() << std::endl;
	} else {
		int worlsize = world.size();
		std::cerr << "World of size " << worlsize << std::endl;
		stream << "       EDGE" << std::endl;
		stream << std::setw(7) << std::right << "DEME "
		<< std::setw(7) << std::left << " LEFT"
		<< std::setw(6) << std::left << "RIGHT";
		if(dimension == 2){
			stream << std::setw(6) << std::left << "UP"
			<< std::setw(6) << std::left << "DOWN";
		}
		stream << std::setw(6) << std::left << "X";
		if(dimension == 2){
			stream << std::setw(6) << std::left << "Y";
		}
		stream << std::setw(12) << std::left << "meanf"
		<< std::setw(12) << std::left << "f(heter)"
		<< std::setw(12) << std::left << "meanHI"
		<< std::setw(12) << std::left << "var(HI)";
		if(number_of_loci * number_of_chromosomes > 1){
			stream << std::setw(12) << std::left << "var(p)"
			<< std::setw(12) << std::left << "LD";
		}

		if(number_of_loci * number_of_chromosomes <= 16){
			for(int ch = 0;ch < number_of_chromosomes;ch++){
				for(int l = 0; l < number_of_loci;l++){
					stream << std::left << "Ch" << ch+1 << "l" << l+1 << std::setw(7) << ' ';
				}
			}
		}
		stream << std::endl;
		saveLinesPerDeme(stream, "summary");
	}
	return 0;
}

double World::getMaterial() const{
	double material = 0;
	if(dimension == 0){
		int pop_size = zeroD_immigrant_pool.size();
		for(int i = 0;i < pop_size;i++){
			material += zeroD_immigrant_pool[i].getBprop();
		}
	} else {
		std::cerr << "getMaterial is not implemented for " << dimension << "D\n";
	}
	return material;
}

int World::getTotalNumberOfBBlocks(){
	int blocks = 0;
	if(dimension == 0){
		for(unsigned int i = 0;i < zeroD_immigrant_pool.size();i++){
			blocks += zeroD_immigrant_pool[i].getNumberOfBBlocks();
		}
	} else {
		std::cerr << "getTotalJunctions is not implemented for " << dimension << "D\n";
	}
	return blocks;
}

double World::getMeanFitness() const{
	double total_fitness = 0, hybrid_index = 0;
	SelectionModel selection_model;
	selection_model.setSelectionPressure(selection);
	selection_model.setBeta(beta);

	if(dimension == 0){
		int pop_size = zeroD_immigrant_pool.size();
		for(int i = 0;i < pop_size;i++){
			hybrid_index = zeroD_immigrant_pool[i].getBprop() / 2;
			total_fitness += selection_model.getFitness(hybrid_index);
		}
		total_fitness = total_fitness / pop_size;
	} else {
		std::cerr << "getMeanFitness is not implemented for " << dimension << "D\n";
	}
	return total_fitness;
}

void World::showOneDeme(int index){
	world[index]->showDeme();
}

int World::saveTheUniverse(std::string type, std::string filename){
	if(type == "raspberrypi"){
		return saveRaspberrypi(std::cout);
	}

	std::ofstream ofile;
	int return_value = 1;

	// for all dims, similar to blocks
	if(type == "backtrace"){
		ofile.open(filename, std::ios_base::app); // Opens file
		return_value = saveBacktrace(ofile);
	} else {
		ofile.open(filename); // Opens file
	}

	if (ofile.fail()){
		return 1;
	}

	// for all dims; if one desires to save std out to separated files instead of one stream
	if(type == "summary"){
		return_value = summary(ofile);
	}
	// for 1D / 2D

	if(type == "hybridIndices" or type == "hybridIndicesJunctions" or type == "complete"){
		return_value = saveLinesPerIndividual(ofile, type);
	}

	// for all dims
	if(type == "blocks"){
		return_value = saveBlocks(ofile);
	}

	ofile.close();
	return return_value;
}

void World::getLD(){
	for (std::map<int, Deme*>::const_iterator i=world.begin(); i!=world.end(); ++i){
		std::cout << i->second->getLD() << '\t';
	}
	std::cout << std::endl;
}

  // // // // // // // //
 // PARAMETER HANDLING//
// // // // // // // //
void World::restart(){
	if(dimension == 0){
		zeroD_immigrant_pool.clear();
	} else {
		clear();
		worldSlave();
	}
	std::cerr << "World is reset." << std::endl;
	return;
}

void World::clear(){
	if(dimension == 0){
		zeroD_immigrant_pool.clear();
	} else {
		for (std::map<int, Deme*>::const_iterator i=world.begin(); i!=world.end(); ++i){
			delete i->second;
		}
		world.clear();
	}
	return;
}

  // // // // //
 //  PRIVATE //
// // // // //

int World::upperBorder(int index, int max_index){
	if(type_of_u_d_edges == "reflexive"){
		if(index == max_index){
			return index;
		} else {
			return index - 1;
		}
	}
	if(type_of_u_d_edges == "wrapping"){
		if(index == max_index){
			return index + (number_of_demes_u_d-1);
		} else {
			return index - 1;
		}
	}
	std::cerr << "Error: The type of upper-down edges is not valid." << std::endl;
	return -1;
}

int World::lowerBorder(int index, int max_index){
	if(type_of_u_d_edges == "reflexive"){
		if(index == max_index + number_of_demes_u_d - 1){
			return index;
		} else {
			return index + 1;
		}
	}
	if(type_of_u_d_edges == "wrapping"){
		if(index == max_index + number_of_demes_u_d  - 1){
			return index - (number_of_demes_u_d-1);
		} else {
			return index + 1;
		}
	}
	std::cerr << "Error: The type of upper-down edges is not valid." << std::endl;
	return -1;
}

int World::sideBorder(int reflexive, int extending){
	if(type_of_l_r_edges == "reflexive"){
		if(reflexive < number_of_demes_l_r * number_of_demes_u_d and reflexive > number_of_demes_u_d){
			return extending;
		} else {
			return reflexive;
		}
	}
	if(type_of_l_r_edges == "extending"){
		return extending;
	}
	if(type_of_l_r_edges == "wrapping"){
		if(reflexive < number_of_demes_l_r * number_of_demes_u_d and reflexive > number_of_demes_u_d){
			return extending;
		} else {
			if(reflexive < number_of_demes_u_d){
				return number_of_demes_l_r * number_of_demes_u_d + reflexive % number_of_demes_u_d;
			} else {
				return reflexive % number_of_demes_u_d;
			}
		}
	}
	if(type_of_l_r_edges == "infinite"){
		if(reflexive < number_of_demes_l_r * number_of_demes_u_d and reflexive > number_of_demes_u_d){
			return extending;
		} else {
			return -8;
		}
	}
	std::cerr << "Error: The type of left-right edges is not valid." << std::endl;
	return -1;
}

bool World::isGameteA(std::vector<Chromosome>& gamete){
	for(int i=0;i<number_of_chromosomes;i++){
		if(gamete[i].isPureA()){
			continue;
		}
		return 0;
	}
	return 1;
}

int World::saveLinesPerIndividual(std::ofstream& ofile, std::string type){
	if(dimension == 0){
		// TO DO
	} else {
		saveLinesPerDeme(ofile, type);
	}
	ofile.close();
	return 0;
}

int World::saveLinesPerDeme(std::ostream& stream, std::string type){
	int comlumn_to_print = index_last_left;
	int deme_to_print = -1;
	int next_column = -1;
	int total_columns = 0;
	// iterates through columns
	while(comlumn_to_print != index_next_right){
		// iterate thought rows
		for(int row = 0; row < number_of_demes_u_d; row++){
			deme_to_print = comlumn_to_print+row;
			if(type == "summary"){
				world[deme_to_print]->streamSummary(stream);
			}
			if(type == "blocks"){
				world[deme_to_print]->streamBlocks(stream);
			}
			if(type == "backtrace"){
				world[deme_to_print]->streamChiasmata(stream);
			}
			if(type == "hybridIndices" or type == "hybridIndicesJunctions" or type == "complete"){
				world[deme_to_print]->streamHIs(stream);
			}
			if(type == "hybridIndicesJunctions" or type == "complete"){
				world[deme_to_print]->streamJunctions(stream);
			}
			if(type == "complete"){
				world[deme_to_print]->streamHeterozygosity(stream);
			}

		}
		next_column = world[comlumn_to_print]->getNeigbours()[1];
		// block for reflexive border
		if(next_column == comlumn_to_print){
			break;
		}
		comlumn_to_print = next_column;
		total_columns++;
	}

	if(type == "backtrace"){
		stream << "# individuals = " << total_columns * number_of_demes_u_d * deme_size  << "; Lvec=[";
		for(int ch = 0; ch < number_of_chromosomes; ch++){
			stream << number_of_loci;
			if ( (ch + 1) < number_of_chromosomes) {
				stream << ",";
			}
		}
		stream << "] SL=[";
		for(int ch = 0; ch < number_of_chromosomes; ch++){
			stream << number_of_selected_loci;
			if ( (ch + 1) < number_of_chromosomes) {
				stream << ",";
			}
		}
		stream << "]" << std::endl;
	}
	return 0;
}

int World::saveRaspberrypi(std::ostream& stream){

	if(world.size() != 64){
		std::cerr << "Wrong number of demes (" << world.size() << "), define 64 demes for raspberrypi file output \n";
		return 1;
	}

	usleep(900);
	stream << 'c' << std::endl;
	usleep(100);

	int index = index_last_left;
	double hybridIndex = 0, LD = 0;
	int R = 0, G = 0, B = 0;
	stream << "m ";
	for(unsigned int i = 0; i < world.size(); i++){
		for(int y = 0; y < number_of_demes_u_d; y++){
			hybridIndex = world[index+y]->getMeanBproportion();
			LD = world[index+y]->getLD();
			R = (int) (hybridIndex * 255 * 0.5);
			G = (int) (std::abs(LD) * 4 * 255);
// 			std::cerr << LD << ' ';
			B = (int) ((1 - hybridIndex) * 255 * 0.5);
			stream << R << ' ' << G << ' ' << B << ' ';
//			R G B 0 255 FF0, 00F
		}
		if(index != index_last_right){
			index = world[index]->getNeigbours()[1];
		} else {
			break;
		}
	}
	stream << std::endl;
	return 0;
}

int World::saveBlocks(std::ofstream& ofile){
	if(dimension == 0){
		streamBlockSizesOf0DWorld(ofile);
	} else {
		// PRINT HEADERS DEME_INDEX CH1h1 CH1h2 ...
		ofile << "Dx,Dy,Di";
		for(int ch = 0; ch < number_of_chromosomes; ch++){
			ofile << "\tC" << ch+1 << "h0\tC" << ch+1 << "h1";
		}
		ofile << std::endl;
		saveLinesPerDeme(ofile, "blocks");
	}
	ofile.close();
	return 0;
}

int World::saveBacktrace(std::ofstream& ofile){
	if(dimension == 0){
		throw std::runtime_error("Backtrace output is not implemented for 0D simulations. If you wish to have this functionality open an issue on https://github.com/KamilSJaron/Conjunction with tag feature_request.");
	} else {
		saveLinesPerDeme(ofile, "backtrace");
	}
	ofile.close();
	return 0;
}

void World::streamBlockSizesOf0DWorld(std::ostream& stream){
	std::vector<int> blockSizes;
	for(unsigned int index = 0; index < zeroD_immigrant_pool.size(); index++){
		zeroD_immigrant_pool[index].getSizesOfBBlocks(blockSizes);
		for(unsigned int i = 0;i < blockSizes.size(); i++){
			stream << std::fixed << blockSizes[i] / double(number_of_loci) << std::endl;
		}
		blockSizes.clear();
	}
	return;
}
