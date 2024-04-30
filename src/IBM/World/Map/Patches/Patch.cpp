#include "IBM/World/Map/Patches/Patch.h"

using namespace std;



Patch::Patch(const int priority, const FormPatch* const &form)
	: priority(priority), form(form)
{
    
}


Patch::~Patch()
{
	delete form;
}


const Coverage Patch::checkCoverage(const Ring &cellEffectiveArea) const
{
	return Geometry::checkCoverage(form->calculateCoveragePercent(cellEffectiveArea));
}

const int Patch::getPriority() const
{
	return priority;
}

const string Patch::showPatchInfo() const
{
	string formInfo = "";

	if(form->getFormInfo().size() != 0)
	{
		formInfo = fmt::format("{}\n", form->getFormInfo());
	}

	return fmt::format(
		"Initializing {} {} patch \"{}\":\n - Priority = {}\n{}{}", 
		form->getFormType(), getContentType(), form->getPatchFilename(), getPriority(), formInfo, getContentInfo()
	);
}


bool Patch::operator<(const Patch& other) const {
	return getPriority() > other.getPriority();
}
