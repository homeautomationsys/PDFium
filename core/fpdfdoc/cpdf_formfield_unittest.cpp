// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfdoc/cpdf_formfield.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(cpdf_formfield, GetFullNameForDict) {
  WideString name = CPDF_FormField::GetFullNameForDict(nullptr);
  EXPECT_TRUE(name.IsEmpty());

  CPDF_IndirectObjectHolder obj_holder;
  CPDF_Dictionary* root = obj_holder.NewIndirect<CPDF_Dictionary>();
  root->SetNewFor<CPDF_Name>("T", "foo");
  name = CPDF_FormField::GetFullNameForDict(root);
  EXPECT_STREQ("foo", name.ToUTF8().c_str());

  CPDF_Dictionary* dict1 = obj_holder.NewIndirect<CPDF_Dictionary>();
  root->SetNewFor<CPDF_Reference>("Parent", &obj_holder, dict1->GetObjNum());
  dict1->SetNewFor<CPDF_Name>("T", "bar");
  name = CPDF_FormField::GetFullNameForDict(root);
  EXPECT_STREQ("bar.foo", name.ToUTF8().c_str());

  CPDF_Dictionary* dict2 = dict1->SetNewFor<CPDF_Dictionary>("Parent");
  name = CPDF_FormField::GetFullNameForDict(root);
  EXPECT_STREQ("bar.foo", name.ToUTF8().c_str());

  CPDF_Dictionary* dict3 = obj_holder.NewIndirect<CPDF_Dictionary>();
  dict2->SetNewFor<CPDF_Reference>("Parent", &obj_holder, dict3->GetObjNum());

  dict3->SetNewFor<CPDF_Name>("T", "qux");
  name = CPDF_FormField::GetFullNameForDict(root);
  EXPECT_STREQ("qux.bar.foo", name.ToUTF8().c_str());

  dict3->SetNewFor<CPDF_Reference>("Parent", &obj_holder, root->GetObjNum());
  name = CPDF_FormField::GetFullNameForDict(root);
  EXPECT_STREQ("qux.bar.foo", name.ToUTF8().c_str());
  name = CPDF_FormField::GetFullNameForDict(dict1);
  EXPECT_STREQ("foo.qux.bar", name.ToUTF8().c_str());
  name = CPDF_FormField::GetFullNameForDict(dict2);
  EXPECT_STREQ("bar.foo.qux", name.ToUTF8().c_str());
  name = CPDF_FormField::GetFullNameForDict(dict3);
  EXPECT_STREQ("bar.foo.qux", name.ToUTF8().c_str());
}
