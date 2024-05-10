#include "grafic_solver.h"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void GraficSolver::_bind_methods() {
	ClassDB::bind_method(D_METHOD("solve", "zile_luna", "zile_100", "numar_persoane", "norme", "ore_noapte", "co", "ore_100"), &GraficSolver::solve);
	ClassDB::bind_method(D_METHOD("get_solutii_gasite"), &GraficSolver::get_solutii_gasite);
	ClassDB::bind_method(D_METHOD("set_nr_solutii_cautate", "nr_solutii_cautate"), &GraficSolver::set_nr_solutii_cautate);
	ClassDB::bind_method(D_METHOD("get_nr_solutii_cautate"), &GraficSolver::get_nr_solutii_cautate);
}

GraficSolver::GraficSolver() {
	load_ture_posibile();
}

GraficSolver::~GraficSolver() {
	// Add your cleanup here.
}

Error GraficSolver::load_ture_posibile() {
	for (int i = 0; i < 15; i++) {
		String path = "res://ture_posibile/" + String::num_int64(i) + ".bin";
		Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
		if (f.is_null()) {
			return ERR_CANT_OPEN;
		}

		PackedInt32Array arr = f->get_buffer(f->get_length()).to_int32_array();
		arr = arr.slice(2); // erase size

		nr_ture_posibile[i] = arr.size();
		for (int j = 0; j < nr_ture_posibile[i]; j++) {
			ture_posibile[i][j] = arr[j];
		}

		// ture_posibile[i].append_array(arr);
	}
	return OK;
}

bool GraficSolver::ok(int v[], int k, int n) {
	if (k >= 2 * n)
		return false;

	if (k < n) { // Calculez zilele

		// Verifica zile luna
		if ((v[k] >> zile_luna) & 0b111)
			return false;

		// Verifica CO
		if ((v[k] & co[k]) != 0)
			return false;

		// Verifica daca sunt ture duble
		for (int i = 0; i < k; i++) {
			if ((v[i] & v[k]) != 0)
				return false;
		}

		// Early exit daca sunt depasite orele de 100%
		if (__builtin_popcount(v[k] & zile_100) * 12 > ore_100[k])
			return false;

	} else { // Calculez noptile

		// TODO: Verifica zile luna

		// Verifica daca se suprapun cu turele de zi
		if ((v[k - n] & v[k]) != 0)
			return false;
		if ((v[k - n] & (v[k] >> 1)) != 0)
			return false;

		// Verifica daca sunt ture duble
		for (int i = n; i < k; i++) {
			if ((v[i] & v[k]) != 0)
				return false;
		}

		// Verifica CO
		if ((co[k - n] & v[k]) != 0)
			return false;
		if ((co[k - n] & (v[k] >> 1)) != 0)
			return false;

		// Verifica ore 100%
		int persOre100 = 0;
		persOre100 += __builtin_popcount(v[k - n] & zile_100) * 12; // Zile
		persOre100 += __builtin_popcount(v[k] & zile_100) * 8; // Nopti
		persOre100 += __builtin_popcount((v[k] >> 1) & zile_100) * 4; // Nopti

		if (persOre100 != ore_100[k - n])
			return false;
	}

	return true;
}

bool GraficSolver::solutie(int k, int n) {
	return k == 2 * n - 1;
}

void GraficSolver::salveaza_solutia(int v[], int n) {
	solutii_baza_curr++;
	nr_solutii_gasite++;

	PackedInt32Array arr;
	arr.resize(n * 2);

	for (int i = 0; i < n * 2; i++) {
		arr[i] = v[i];
	}

	solutii_gasite.append(arr);
}

void GraficSolver::back(int v[], int k, int n) {
	int nr_ture;

	if (k < n)
		// Zile
		nr_ture = norme[k] / 8;
	else
		// Nopti
		nr_ture = ore_noapte[k - n] / 8;

	for (int i = 0; i < nr_ture_posibile[nr_ture]; i++) {
		if (nr_solutii_gasite >= nr_solutii_cautate)
			return;
		if (solutii_baza_curr >= MAX_SOLUTII_BAZA)
			return;

		v[k] = ture_posibile[nr_ture][i];
		if (ok(v, k, n)) {
			if (solutie(k, n)) {
				salveaza_solutia(v, n);
			} else {
				back(v, k + 1, n);
				if (k < n)
					solutii_baza_curr = 0;
			}
		}
	}
}

void GraficSolver::solve(int p_zile_luna, int p_zile_100, int p_numar_persoane, PackedInt32Array p_norme, PackedInt32Array p_ore_noapte, PackedInt32Array p_co, PackedInt32Array p_ore_100) {
	zile_luna = p_zile_luna;
	zile_100 = p_zile_100;
	numar_persoane = p_numar_persoane;
	norme = p_norme;
	ore_noapte = p_ore_noapte;
	co = p_co;
	ore_100 = p_ore_100;

	int v[numar_persoane * 2] = { 0 };

	back(v, 0, numar_persoane);
}

Array GraficSolver::get_solutii_gasite() {
	return solutii_gasite;
}

void GraficSolver::set_nr_solutii_cautate(int p_nr_solutii_cautate) {
	nr_solutii_cautate = p_nr_solutii_cautate;
}

int GraficSolver::get_nr_solutii_cautate() {
	return nr_solutii_cautate;
}
