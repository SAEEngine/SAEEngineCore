#include "SAEEngineCore_glObject.h"

#if false
namespace sae::engine::core
{
	bool glWidget::on_initialize()
	{
		this->get_artist()->insert(this);
		return true;
	};
	void glWidget::on_destroy()
	{
		this->get_artist()->remove(this);
	};

	void glWidget::refresh()
	{
		this->get_artist()->refresh(this);
	};

}
#endif
