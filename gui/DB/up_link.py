import sqlalchemy as sa
from models import Base

class UpLink(Base):
    __tablename__ = 'up_link'
    id = sa.Column('id', sa.Integer(), primary_key=True, autoincrement=True)
    ip = sa.Column('ip', sa.String(length=64), nullable=False)
    port = sa.Column('port', sa.Integer(), nullable=False)
    data = sa.Column('data', sa.String(1046), nullable=False)
    updated_at = sa.Column('updated_at', sa.DateTime(), nullable=False)
    flag = sa.Column('flag', sa.Integer())
