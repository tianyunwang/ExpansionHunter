//
// Expansion Hunter
// Copyright 2016-2019 Illumina, Inc.
// All rights reserved.
//
// Author: Egor Dolzhenko <edolzhenko@illumina.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//

#pragma once

#include <iostream>
#include <string>

#include "common/Parameters.hh"
#include "output/BamletWriter.hh"
#include "region_spec/LocusSpecification.hh"
#include "workflow/LocusFindings.hh"

namespace ehunter
{

SampleFindings htsSeekingSampleAnalysis(
    const InputPaths& inputPaths, Sex sampleSex, const RegionCatalog& regionCatalog, BamletWriterPtr bamletWriter);

}
