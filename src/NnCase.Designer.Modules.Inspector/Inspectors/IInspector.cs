﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NnCase.Designer.Modules.Inspector.Inspectors
{
    public interface IInspector
    {
        string Name { get; }

        bool IsReadOnly { get; }
    }
}
