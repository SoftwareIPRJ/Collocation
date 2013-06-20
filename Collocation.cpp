//========================================================================================
// Name        : Collocation.cpp
// Author      : Joao Flavio Vieira de Vasconcellos
// Version     : 1.0
// Description : Solucao de um problema utilizando o método de colocação (collocation)
//
// Copyright   : Copyright (C) <2012>  Joao Flavio Vasconcellos (jflavio at iprj.uerj.br)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//========================================================================================


//============================================================================
// C++ Includes
//============================================================================
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using std::cout;
using std::endl;
using std::setw;

//============================================================================
// Petsc Includes
//============================================================================
#include <petsc.h>


//============================================================================
// Boost Includes
//============================================================================


//============================================================================
// Classes utilizadas
//============================================================================


class DadosModelo
{
public:

PetscInt        n,
                iter_max;

PetscReal       lx,
                beta,
                dt,
                t_max,
                k22,
                k44;

Vec             x,
                phi,
                phi_a,          // Phi analitico
                sp,
                B;

Mat             A;

KSP             ksp;


};





//============================================================================
// Variaveis global
//============================================================================

static char help[] = "Programa de teste de: collocation method\n";

//============================================================================
// Header das funções
//============================================================================


#undef __FUNCT__
#define __FUNCT__ "int main(int, char**)"
int main(int argc, char** argv)
{
PetscInt        LZISE(90);

    std::cout << help;
    for (unsigned int i=0; i < LZISE; i++) std::cout << "=";
   std::cout << std::endl << std::endl << std::endl;

//============================================================================
// Iniciando o petsc
//============================================================================

PetscErrorCode ierr;

    ierr = PetscInitialize( &argc, &argv,(char *)0, help); CHKERRQ(ierr);

//============================================================================
// Leitura do arquivo DiffusionRetention.ini
//============================================================================

const std::string FILENAME ("Collocation.ini");
std::fstream	input;

	try
	{
		input.open(FILENAME.c_str(), std::ios::in);
		if( !input ) throw std::ios::failure( "Erro ao abrir o arquivo " + FILENAME ) ;

	}

 	catch (const std::exception &excep)
	{
		std::cerr << excep.what() << '\n' ;
                std::cerr << "Rotina : " << __FUNCT__ << std::endl;
		std::cerr << "Arquivo : " << __FILE__ << std::endl;
		std::cerr << "Linha : " << (__LINE__ - 10) <<std:: endl;
                std::cerr << "Execucao cancelada" << std::endl;
		abort();
	}


std::string		ini_line;
int                     line_counter(0);

DadosModelo             modelo;

	while (getline (input, ini_line))
	{

            	try
		{
                       std::istringstream strin(ini_line);
                       	if (!strin) throw std::ios::failure( "Entrada de dados errada no arquivo:" + FILENAME ) ;
			if (ini_line.size() > 2)
			{
                            	line_counter++;
                                switch (line_counter)
				{
                                		default :
						std::cerr << "Dados errados no arquivo: " << FILENAME << std::endl;
						std::cerr << "Execucao cancelada" << std::endl;
						std::cerr << "Rotina : " << __FUNCT__ << std::endl;
						std::cerr << "Arquivo : " << __FILE__ << std::endl;
						std::cerr << "Linha : " << (__LINE__ - 7) << std::endl;
						abort();
                                case 1:  	strin >> modelo.n;
     						break;
//
//				case 1:  	strin >> modelo.k22 >> modelo.k44;
//						break;
//
//                                case 2:  	strin >> modelo.beta;
//     						break;
//
//
//
//                                case 4:  	strin >> modelo.lx;
//     						break;
//
//                                case 5:  	strin >> modelo.dt;
//     						break;
//
//                                case 6:  	strin >> modelo.t_max;
//     						break;
//
//                                case 7:  	strin >> modelo.iter_max;
//     						break;

                                }
                        }
                }
                catch (const std::exception &excep)
		{
                        std::cerr << excep.what() << '\n' ;
			std::cerr << "Execucao cancelada" << std::endl;
			std::cerr << "Rotina : " << __FUNCT__ << std::endl;
			std::cerr << "Arquivo : " << __FILE__ << std::endl;
			std::cerr << "Linha : " << (__LINE__ - 7) <<std:: endl;
			abort();
		}
        }


std::ios_base::fmtflags ff(std::cout.flags());

        std::cout.flags ( std::ios::left |
			  std::ios::showpoint );

	std::cout.precision(4);



        std::cout << "Dados do arquivo: " << FILENAME << "\n";
        for (int i=0; i<LZISE; i++) std::cout <<"="; std::cout <<"\n";
//        std::cout << "K1 .......................................... " << std::setw(15) << std::scientific << modelo.k22 << "\n";
//        std::cout << "K2 .......................................... " << std::setw(15) << modelo.k44 << "\n";
//        std::cout << "Beta ........................................ " << std::setw(15) << std::fixed << modelo.beta << "\n";
//        std::cout << "Lx .......................................... " << std::setw(15) << modelo.lx << "\n";
        std::cout << "Numero de nos (n) ........................... " << std::setw(15) << modelo.n << "\n";
//        std::cout << "dx .......................................... " << std::setw(15) << modelo.dx << "\n";
//        std::cout << "dt .......................................... " << std::setw(15) << modelo.dt << "\n";
//        std::cout << "t_max ....................................... " << std::setw(15) << modelo.t_max << "\n";
//        std::cout << "iter_max .................................... " << std::setw(6) << modelo.iter_max << "\n\n";


//============================================================================
// Inicio
//============================================================================

        for (int i=0; i<LZISE; i++) std::cout <<"="; std::cout <<"\n\n";
        std::cout.flags(ff);



//============================================================================
// Dimensionamento dos vetores
//============================================================================


        ierr = VecCreate(PETSC_COMM_WORLD, &modelo.x); CHKERRQ(ierr);
        ierr = VecSetSizes(modelo.x, PETSC_DECIDE, modelo.n); CHKERRQ(ierr);
        ierr = VecSetFromOptions(modelo.x); CHKERRQ(ierr);
//
//        ierr = VecDuplicate(modelo.phi, &modelo.phi_a); CHKERRQ(ierr);
//        ierr = VecDuplicate(modelo.phi, &modelo.sp); CHKERRQ(ierr);
//        ierr = VecDuplicate(modelo.phi, &modelo.B); CHKERRQ(ierr);
//
//        ierr = MatCreateSeqAIJ(PETSC_COMM_SELF, modelo.nx, modelo.nx, 5, PETSC_NULL, &modelo.A); CHKERRQ(ierr);
//        ierr = MatSetType(modelo.A, MATSEQAIJ); CHKERRQ(ierr);
//        ierr = MatSetOption(modelo.A, MAT_STRUCTURALLY_SYMMETRIC, PETSC_TRUE); CHKERRQ(ierr);
//
////============================================================================
//// Condicao inicial e Calculo dos Coeficientes
////============================================================================
//
//

//============================================================================
// Geracao da malha entre -1 <= x <= -1 (malha igualmente espaçada)
//============================================================================

PetscScalar             *xm;
PetscReal               dx = 2.0l / (modelo.n - 1l);

    ierr = VecGetArray(modelo.x, &xm); CHKERRQ(ierr);
    xm[0] = -1.0l;

    for (int i=1; i < modelo.n; i++)
    {
        xm[i] = xm[i-1] + dx;
    }

    std::cout << "Dados da malha "  << "\n";
    for (unsigned int i=0; i < LZISE; i++) std::cout << "=";
    std::cout << std::endl;

    ff = cout.flags();

    cout.setf (std::ios::fixed);
    cout.precision(5);

    for (int i=0; i < modelo.n; i++)
    {
        cout <<  setw(5)  << i
             <<  setw(15) << xm[i]
             <<  endl;;
    }

    cout.flags(ff);
    for (unsigned int i=0; i < LZISE; i++) std::cout << "=";
    std::cout << std::endl;


    ierr = VecRestoreArray(modelo.x, &xm); CHKERRQ(ierr);



////============================================================================
//// Apagando os vetores e matriz
////============================================================================
    ierr = VecDestroy(&modelo.x); CHKERRQ(ierr);
//    ierr = VecDestroy(&modelo.phi); CHKERRQ(ierr);
//    ierr = VecDestroy(&modelo.phi_a); CHKERRQ(ierr);
//    ierr = VecDestroy(&modelo.B); CHKERRQ(ierr);
//    ierr = VecDestroy(&modelo.sp); CHKERRQ(ierr);
//    ierr = MatDestroy(&modelo.A); CHKERRQ(ierr);
//    ierr = KSPDestroy(&modelo.ksp); CHKERRQ(ierr);

//============================================================================
// Finalizando o petsc
//============================================================================

    std::cout << endl << endl << endl;
    std::cout << help;
    for (unsigned int i=0; i < LZISE; i++) std::cout << "=";
    std::cout << std::endl;

    ierr = PetscFinalize(); CHKERRQ(ierr);

    return EXIT_SUCCESS;
}

