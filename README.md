
# Weaver

![Deploy release status](https://github.com/jordiSpider/Weaver/actions/workflows/deploy-release.yml/badge.svg)
![License](https://img.shields.io/badge/license-MIT-green)
![C++](https://img.shields.io/badge/C++17-blue)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey)

---

## Table of Contents

- [Weaver](#weaver)
  - [Table of Contents](#table-of-contents)
  - [🌍 Overview](#-overview)
  - [🧭 Getting Started](#-getting-started)
    - [🧑‍💻 For End Users](#-for-end-users)
    - [🧠 For Developers](#-for-developers)
  - [📘 Documentation](#-documentation)
  - [🤝 Contributing](#-contributing)
  - [📄 License](#-license)
  - [References:](#references)

---

## 🌍 Overview

> [!WARNING]
> This code is currently under test and a TRACE [^1] document being written.

Weaver is a spatially-explicit next-generation individual-based model (sensu [^2]) that links animal evolvable multidimensional functional trait diversity to food webs. Basal resources (fungi, bacteria or plants) grow according to discrete-time difference equations or chemostats, and primary consumers (microbivores or herbivores) are individuals that feed on these basal resources and in turn are fed upon by predators. The latter may also feed on each other both intra- and interspecifically, thus effectively weaving food webs.

This simulation platform allows the in silico investigation of eco-evolutionary dynamics of animal metacommunities embedded in food webs that in turn may vary in configuration through the landscape. Spatial-temporal landscape heterogeneity in productivity, water availability and temperature can be parameterized very realistically.

Since it was first published [^3], with many of the algorithms following from a simpler simulation platform [^4], we have been working on improving the code, fine tuning and expanding the functionalities, and extending the simulation scenarios from soil arthropods to other systems: soil nematodes, biological pest control in crops, headwater streams, and Cretaceous dinosaurs. 

Among the new biological functionalities we can highlight: a) Individuals grow following known growth curves from published literature; to that end, b) both basal resources and individuals can grow depending on temperature performance curves (TPCs), from which many other traits can depend; c) Using an octree algorithm the landscape has obstacles of different grain, from which smaller prey can scape predators; d) individuals have prey preferences, memory and can perform behavioral switching and ontogenetic niche shifts; and all these are linked to traits that can evolve. e) the algorithms for patch use have been improved, and animals can now be forced to move now through habitat domains [^5] and consider conspecific abundances, in addition to assess predation risk and resource availability. 

Among the new simulation functionalities, we can highlight: a) The code has been serialized and very long simulations can now re-start were they finished, b) High flexibility to choose outputs and avoid hard drive overuse have been implemented, c) some debugging tools when testing newly parameterized individuals have been included.

Finally, the code has been optimized using a “building blocks” philosophy [^6], making most individual algorithms easily portable to other platforms. This simulation platform will be made Open Source and include a User Manual, hopefully by the end of 2026.

This research has been possible thanks to grants Weaver - CGL2014-61314-EXP; ENGIWEB - CGL2015-66192-R (Spanish Ministry of Economy and Competitiveness), SPILL-ISLAND – PID2019-103863RB-I00 (funded by MCIN/AEI /10.13039/501100011033, the Spanish Research Agency) and SIMULPEST – TED2021-129653B-I00 (funded by MCIN/AEI /10.13039/501100011033 Spanish Research Agency, and European Union NextGenerationEU/PRTR), Investigo program - Junta de Andalucía - PRTR - NextGenerationEU.

---

## 🧭 Getting Started

### 🧑‍💻 For End Users

If you only want to **run the program**, follow these simple steps:

1. Download the latest release
  You can find precompiled installers for all supported operating systems in the [Releases](https://github.com/jordiSpider/Weaver/releases) section.
  Simply download the installer for your platform and follow the [Instructions](https://jordiSpider.github.io/Weaver/user_guide/installation.html).

1. Run the program
  Once installed, launch the program from the Start Menu, Applications folder, or terminal. You can run Weaver in GUI or CLI mode:

```bash
# GUI mode
Weaver-GUI.exe -I path/to/config -O path/to/output_folder

# CLI mode
Weaver-Non-GUI.exe -I path/to/config -O path/to/output_folder
```

3. Configuration
  Some functionality may require configuration files or runtime parameters. Refer to the [User Guide](https://jordiSpider.github.io/Weaver/user_guide/usage.html) section in the online documentation for detailed usage instructions.

### 🧠 For Developers

If you want to contribute to the project or build it from source, please refer to the [Developer Guide](https://jordiSpider.github.io/Weaver/developer_guide/build_system.html).

---

## 📘 Documentation

For detailed information, please refer to the [Documentation](https://jordiSpider.github.io/Weaver/) section. It includes user guides, developer references, and API documentation.

---

## 🤝 Contributing

Developers are welcome to contribute new features or improvements. See [Contribution](https://jordiSpider.github.io/Weaver/developer_guide/contribution.html) section for more details.

---

## 📄 License

This project is licensed under the **MIT License**.
See [LICENSE](LICENSE) for details.

## References:

[^1]: Grimm et al. 2014 Ecological Modelling 280:129-139.
[^2]: Grimm et al. 2017 Ecosystems 20:229-236.
[^3]: Moya-Laraño J et al. 2014 Adv Ecol Res 50:75-143.
[^4]: Moya-Laraño J et al. 2012 Adv Ecol Res 47:1-80.
[^5]: Schmitz, O. J. 2005. In P. Barbosa & I. Castellanos (Eds.), Ecology of Predator-Prey Interactions (pp. 256-278). Oxford University Press.
[^6]: Berger et al. 2024. Ecological Modelling and Software 175:106003.
