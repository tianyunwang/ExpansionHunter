//
// Expansion Hunter
// Copyright (c) 2016 Illumina, Inc.
//
// Author: Egor Dolzhenko <edolzhenko@illumina.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "graphs/graph_mapping.h"

#include "gtest/gtest.h"

#include "graphs/graph.h"
#include "graphs/graph_builders.h"
#include "graphs/graph_mapping_operations.h"
#include "graphs/path.h"

using std::list;
using std::string;
using std::vector;

TEST(InitializingGraphMapping, CompatiblePath_GraphMappingCreated) {
  GraphSharedPtr graph_ptr = MakeDeletionGraph("AAAA", "TTGG", "TTTT");

  {
    GraphPath path(graph_ptr, 3, {0, 1, 2}, 3);
    vector<Mapping> mappings = {Mapping(3, "1M", "A", "AAAA"),
                                Mapping(0, "4M", "TTGG", "TTGG"),
                                Mapping(0, "4M", "TTTT", "TTTT")};
    EXPECT_NO_THROW(GraphMapping(path, mappings));
  }

  {
    GraphPath path(graph_ptr, 2, {1}, 2);
    vector<Mapping> mappings = {Mapping(2, "1M", "G", "TTGG")};
    EXPECT_NO_THROW(GraphMapping(path, mappings));
  }
}

TEST(InitializingGraphMapping, IncompatiblePath_ExceptionThrown) {
  GraphSharedPtr graph_ptr = MakeDeletionGraph("AAAA", "TTGG", "TTTT");
  GraphPath path(graph_ptr, 2, {0, 1, 2}, 3);
  {
    vector<Mapping> mappings = {Mapping(3, "1M", "A", "AAAA"),
                                Mapping(0, "4M", "TTGG", "TTGG"),
                                Mapping(0, "4M", "TTTT", "TTTT")};
    EXPECT_ANY_THROW(GraphMapping(path, mappings));
  }

  {
    vector<Mapping> mappings = {Mapping(2, "2M", "AA", "AAAA"),
                                Mapping(0, "4M", "TTGG", "TTGG"),
                                Mapping(0, "3M", "TTT", "TTTT")};
    EXPECT_ANY_THROW(GraphMapping(path, mappings));
  }
}

TEST(GettingNumMatchesInGraphMapping, TypicalGraphMapping_GotNumMatches) {
  GraphSharedPtr graph_ptr = MakeDeletionGraph("AAAA", "TTGG", "TTTT");
  const string query = "AAAATTCCC";
  GraphMapping graph_mapping =
      DecodeFromString(0, "0[4M]1[2M3S]", query, graph_ptr);
  EXPECT_EQ((int32_t)6, graph_mapping.NumMatches());
}

TEST(GettingGraphMappingSeqs, TypicalGraphMapping_GotQueryAndReference) {
  GraphSharedPtr graph_ptr = MakeDeletionGraph("AAAA", "TTGG", "TTTT");
  const string query = "AAAATTCCC";
  GraphMapping graph_mapping =
      DecodeFromString(0, "0[4M]1[2M3S]", query, graph_ptr);
  EXPECT_EQ("AAAATT", graph_mapping.Query());
  EXPECT_EQ("AAAATT", graph_mapping.Reference());
}

TEST(GettingGraphMappingSpans, TypicalGraphMapping_GotQueryAndReferenceSpans) {
  GraphSharedPtr graph_ptr = MakeDeletionGraph("AAAA", "TTGG", "TTTT");
  const string query = "AAAATTCCC";
  GraphMapping graph_mapping =
      DecodeFromString(0, "0[4M]1[2M3S]", query, graph_ptr);
  EXPECT_EQ((int32_t)9, graph_mapping.QuerySpan());
  EXPECT_EQ((int32_t)6, graph_mapping.ReferenceSpan());
}

TEST(AccessingNodeMappingsByIndex, TypicalGraphMapping_NodeMappingsAccessed) {
  GraphSharedPtr graph_ptr = MakeDeletionGraph("AAAA", "TTGC", "TTTT");
  const string query = "AAAATTCCC";
  GraphMapping graph_mapping =
      DecodeFromString(0, "0[4M]1[2M3S]", query, graph_ptr);
  EXPECT_EQ(Mapping(0, "4M", "AAAA", "AAAA"), graph_mapping[0]);
  EXPECT_EQ(Mapping(0, "2M3S", "TTCCC", "TTGG"), graph_mapping[1]);
}

TEST(GettingIndexesOfNode, TypicalMapping_IndexesObtained) {
  GraphSharedPtr graph_ptr = MakeStrGraph("AAAACC", "CCG", "ATTT");
  const string read = "CCCCGCCGAT";
  GraphMapping mapping =
      DecodeFromString(4, "0[2M]1[3M]1[3M]2[2M]", read, graph_ptr);
  const list<int32_t> left_flank_indexes = {0};
  const list<int32_t> repeat_unit_indexes = {1, 2};
  const list<int32_t> right_flank_indexes = {3};
  EXPECT_EQ(left_flank_indexes, mapping.GetIndexesOfNode(0));
  EXPECT_EQ(repeat_unit_indexes, mapping.GetIndexesOfNode(1));
  EXPECT_EQ(right_flank_indexes, mapping.GetIndexesOfNode(2));
}

TEST(GettingIndexesOfNode, NodeNotInMapping_EmptyListReturned) {
  GraphSharedPtr graph_ptr = MakeStrGraph("AAAACC", "CCG", "ATTT");
  const string read = "ACCCCG";
  GraphMapping mapping = DecodeFromString(3, "0[3M]1[3M]", read, graph_ptr);
  const list<int32_t> empty_list;
  EXPECT_EQ(empty_list, mapping.GetIndexesOfNode(2));
  EXPECT_EQ(empty_list, mapping.GetIndexesOfNode(4));
}

TEST(CheckingIfMappingOverlapsNode, TypicalMapping_ChecksPerformed) {
  GraphSharedPtr graph_ptr = MakeStrGraph("AAAACC", "CCG", "ATTT");
  const string read = "ACCCCG";
  GraphMapping mapping = DecodeFromString(3, "0[3M]1[3M]", read, graph_ptr);
  EXPECT_TRUE(mapping.OverlapsNode(0));
  EXPECT_TRUE(mapping.OverlapsNode(1));
  EXPECT_FALSE(mapping.OverlapsNode(2));
  EXPECT_FALSE(mapping.OverlapsNode(3));
}

TEST(EncodingGraphMapping, TypicalGraphMapping_CigarStringObtained) {
  GraphSharedPtr graph_ptr = MakeStrGraph("AAAACC", "CCG", "ATTT");
  const string read = "CCCCGCCGAT";
  const string cigar_string = "0[2M]1[3M]1[3M]2[2M]";
  GraphMapping mapping = DecodeFromString(4, cigar_string, read, graph_ptr);

  ASSERT_EQ(cigar_string, mapping.GetCigarString());
}
