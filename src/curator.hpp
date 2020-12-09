#ifndef _CYLAKS_CURATOR_HPP_
#define _CYLAKS_CURATOR_HPP_
#include "definitions.hpp"
#include "filament_manager.hpp"
#include "protein_manager.hpp"
#include "system_namespace.hpp"
#include "system_parameters.hpp"
#include "system_rng.hpp"

class Curator {
private:
  struct DataFile {
    Str name_{"example"};
    Str filename_{"simName_example.file"};
    FILE *fileptr_;
    DataFile() {}
    DataFile(Str name) : name_{name} {
      filename_ = Sys::name_ + "_" + name_ + ".file";
      fileptr_ = fopen(filename_.c_str(), "w");
      if (fileptr_ == nullptr) {
        printf("Error; cannot open '%s'\n", filename_.c_str());
        exit(1);
      }
    }
    template <typename DATA_T> void Write(DATA_T *array, size_t count) {
      size_t n_chars_written{fwrite(array, sizeof(DATA_T), count, fileptr_)};
      if (n_chars_written < count) {
        printf("Error writing to '%s'\n", filename_.c_str());
        exit(1);
      }
    }
  };
  UMap<Str, DataFile> data_files_;
  size_t n_steps_per_snapshot_;

  SysTimepoint start_time_;

public:
  ProteinManager proteins_;
  FilamentManager filaments_;

  SysParams params_;
  SysRNG gsl_;

private:
  void CheckArgs(int argc, char *agrv[]);
  void GenerateLog();
  void ParseParameters();
  void InitializeSimulation();
  void GenerateDataFiles();

  void AddDataFile(Str name) { data_files_.emplace(name, DataFile(name)); }

  void CheckPrintProgress();
  void OutputData();

public:
  Curator(int argc, char *argv[]) {
    CheckArgs(argc, argv);
    GenerateLog();
    ParseParameters();
    InitializeSimulation();
    GenerateDataFiles();
  }
  void EvolveSimulation() {
    proteins_.RunKMC();
    filaments_.RunBD();
    CheckPrintProgress();
    OutputData();
  }
};
#endif