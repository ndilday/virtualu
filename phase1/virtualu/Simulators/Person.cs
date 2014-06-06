using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace virtualu.Simulators
{
    public enum Gender
    {
        Male,
        Female
    }

    public enum EthnicGroup
    {
        Minority,
        NonMinority
    }

    class Person
    {
        public readonly short department_recno;                       // this means major field of the student
        public readonly StudentLevel student_level;
        public readonly EthnicGroup gender_ethnic_group;
        public readonly Gender gender;
    }
}
